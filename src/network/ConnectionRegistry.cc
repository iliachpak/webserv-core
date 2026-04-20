#include "network/ConnectionRegistry.hh"

namespace net
{
    ConnectionRegistry::ConnectionRegistry()
    {
    }

    ConnectionRegistry::~ConnectionRegistry()
    {
        clear();
    }

    bool ConnectionRegistry::add(Connection* conn)
    {
        if (conn == NULL)
            return false;

        SocketFD fd = conn->getFd();
        if (contains(fd))
            return false;

        _connections[fd] = conn;
        return true;
    }

    bool ConnectionRegistry::remove(SocketFD fd)
    {
        std::map<SocketFD, Connection*>::iterator it = _connections.find(fd);
        if (it == _connections.end())
            return false;

        delete it->second;
        _connections.erase(it);
        return true;
    }

    Connection* ConnectionRegistry::get(SocketFD fd) const
    {
        std::map<SocketFD, Connection*>::const_iterator it = _connections.find(fd);
        if (it == _connections.end())
            return NULL;

        return it->second;
    }

    bool ConnectionRegistry::contains(SocketFD fd) const
    {
        return _connections.find(fd) != _connections.end();
    }

    std::size_t ConnectionRegistry::size() const
    {
        return _connections.size();
    }

    void ConnectionRegistry::clear()
    {
        for (std::map<SocketFD, Connection*>::iterator it = _connections.begin();
             it != _connections.end();
             ++it)
        {
            delete it->second;
        }

        _connections.clear();
    }
}
