#include "network/EventLoop.hh"

#include <cerrno>

#include <sys/socket.h>
#include <unistd.h>

namespace net
{
    EventLoop::EventLoop(std::size_t maxClients)
        : _handler(NULL),
          _maxClients(maxClients),
          _running(false)
    {
    }

    EventLoop::~EventLoop()
    {
        stop();

        for (std::set<SocketFD>::iterator it = _serverSockets.begin();
             it != _serverSockets.end();
             ++it)
        {
            _poller.remove(*it);
            SocketUtils::closeSocket(*it);
        }

        _serverSockets.clear();
        _registry.clear();
    }

    bool EventLoop::addServer(const ListenerConfig& listener)
    {
        return addServer(listener.host, listener.port, listener.backlog);
    }

    bool EventLoop::addServer(const std::string& host,
                                   unsigned short port,
                                   int backlog)
    {
        SocketFD serverFd = SocketUtils::createServerSocket(host, port, backlog);
        if (serverFd == INVALID_SOCKET_FD)
            return false;

        if (!_poller.add(serverFd, true, false))
        {
            SocketUtils::closeSocket(serverFd);
            return false;
        }

        _serverSockets.insert(serverFd);
        return true;
    }

    bool EventLoop::addConnection(Connection* conn)
    {
        if (conn == NULL)
            return false;

        SocketFD fd = conn->getFd();

        if (!_poller.add(fd, conn->wantsRead(), conn->wantsWrite()))
        {
            delete conn;
            return false;
        }

        if (!_registry.add(conn))
        {
            _poller.remove(fd);
            delete conn;
            return false;
        }

        return true;
    }

    bool EventLoop::removeConnection(SocketFD fd)
    {
        bool removedFromPoller = _poller.remove(fd);
        bool removedFromRegistry = _registry.remove(fd);
        return removedFromPoller && removedFromRegistry;
    }

    bool EventLoop::isServerSocket(SocketFD fd) const
    {
        return _serverSockets.find(fd) != _serverSockets.end();
    }

    std::size_t EventLoop::connectionCount() const
    {
        return _registry.size();
    }

    void EventLoop::setHandler(INetworkHandler* handler)
    {
        _handler = handler;
    }

    void EventLoop::run()
    {
        _running = true;

        while (_running)
            runOnce(100);
    }

    void EventLoop::runOnce(int timeoutMs)
    {
        std::vector<NetworkEvent> events;
        runOnce(timeoutMs, events);
    }

    void EventLoop::runOnce(int timeoutMs, std::vector<NetworkEvent>& events)
    {
        if (!_poller.wait(events, timeoutMs))
            return;

        for (std::vector<NetworkEvent>::iterator it = events.begin();
             it != events.end();
             ++it)
        {
            if (isServerSocket(it->fd))
            {
                if (it->type == EVENT_READ)
                    handleServerReadable(it->fd);
                continue;
            }

            handleClientEvent(*it);
        }
    }

    void EventLoop::stop()
    {
        _running = false;
    }

    void EventLoop::handleServerReadable(SocketFD serverFd)
    {
        while (true)
        {
            SocketFD clientFd = accept(serverFd, NULL, NULL);
            if (clientFd < 0)
            {
                if (errno == EINTR)
                    continue;
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                return;
            }

            if (_registry.size() >= _maxClients)
            {
                SocketUtils::closeSocket(clientFd);
                continue;
            }

            if (!SocketUtils::setNonBlocking(clientFd))
            {
                SocketUtils::closeSocket(clientFd);
                continue;
            }

            Connection* conn = new Connection(clientFd);
            if (!addConnection(conn))
                continue;

            if (_handler != NULL)
            {
                Connection* addedConn = _registry.get(clientFd);
                if (addedConn != NULL)
                {
                    _handler->onConnectionOpen(*addedConn);

                    if (addedConn->isClosed())
                    {
                        _handler->onConnectionClose(clientFd);
                        removeConnection(clientFd);
                        continue;
                    }

                    updateInterest(addedConn);
                }
            }
        }
    }

    void EventLoop::handleClientEvent(const NetworkEvent& event)
    {
        Connection* conn = _registry.get(event.fd);
        if (conn == NULL)
            return;

        if (event.type == EVENT_READ)
        {
            conn->onReadable();

            if (!conn->isClosed() && _handler != NULL && !conn->getReadBuffer().empty())
                _handler->onConnectionData(*conn);
        }
        else if (event.type == EVENT_WRITE)
        {
            conn->onWritable();

            if (!conn->isClosed() && _handler != NULL)
                _handler->onConnectionWritable(*conn);
        }
        else
        {
            conn->onDisconnect();
        }

        if (conn->isClosed())
        {
            if (_handler != NULL)
                _handler->onConnectionClose(event.fd);
            removeConnection(event.fd);
            return;
        }

        updateInterest(conn);
    }

    void EventLoop::updateInterest(Connection* conn)
    {
        if (conn == NULL)
            return;

        SocketFD fd = conn->getFd();
        if (_poller.modify(fd, conn->wantsRead(), conn->wantsWrite()))
            return;

        conn->onDisconnect();

        if (_handler != NULL)
            _handler->onConnectionClose(fd);

        removeConnection(fd);
    }
}
