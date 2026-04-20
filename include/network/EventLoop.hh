#ifndef EPOLL_EVENT_LOOP_HH
#define EPOLL_EVENT_LOOP_HH

#include <set>
#include <string>
#include <vector>

#include "network/ConnectionRegistry.hh"
#include "network/Poller.hh"
#include "network/INetworkHandler.hh"
#include "network/ListenerConfig.hh"
#include "network/Connection.hh"
#include "network/SocketUtils.hh"

namespace net
{
    class EventLoop
    {
    public:
        explicit EventLoop(std::size_t maxClients);
        ~EventLoop();

        bool addServer(const ListenerConfig& listener);
        bool addServer(const std::string& host, unsigned short port, int backlog);

        bool addConnection(Connection* conn);
        bool removeConnection(SocketFD fd);

        bool isServerSocket(SocketFD fd) const;

        std::size_t connectionCount() const;

        void setHandler(INetworkHandler* handler);

        void run();
        void runOnce(int timeoutMs);
        void runOnce(int timeoutMs, std::vector<NetworkEvent>& events);
        void stop();

    private:
        void handleServerReadable(SocketFD serverFd);
        void handleClientEvent(const NetworkEvent& event);
        void updateInterest(Connection* conn);

    private:
        Poller        _poller;
        ConnectionRegistry _registry;
        std::set<SocketFD> _serverSockets;
        INetworkHandler*   _handler;
        std::size_t        _maxClients;
        bool               _running;
    };
}

#endif
