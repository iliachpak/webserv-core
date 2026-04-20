#ifndef CONNECTION_REGISTRY_HH
#define CONNECTION_REGISTRY_HH

#include <cstddef>
#include <map>

#include "network/NetworkTypes.hh"
#include "network/Connection.hh"

namespace net
{
    class ConnectionRegistry
    {
    public:
        ConnectionRegistry();
        ~ConnectionRegistry();

        bool add(Connection* conn);
        bool remove(SocketFD fd);

        Connection* get(SocketFD fd) const;
        bool contains(SocketFD fd) const;
        std::size_t size() const;

        void clear();

        typedef std::map<SocketFD, Connection*>::iterator iterator;
        iterator begin() { return _connections.begin(); }
        iterator end() { return _connections.end(); }

    private:
        ConnectionRegistry(const ConnectionRegistry&);
        ConnectionRegistry& operator=(const ConnectionRegistry&);

    private:
        std::map<SocketFD, Connection*> _connections;
    };
}

#endif
