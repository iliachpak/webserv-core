#ifndef I_NETWORK_HANDLER_HH
#define I_NETWORK_HANDLER_HH

#include "network/NetworkTypes.hh"

namespace net
{
    class Connection;

    class INetworkHandler
    {
    public:
        virtual ~INetworkHandler() {}

        virtual void onConnectionOpen(Connection&)
        {
        }

        virtual void onConnectionData(Connection&)
        {
        }

        virtual void onConnectionWritable(Connection&)
        {
        }

        virtual void onConnectionClose(SocketFD)
        {
        }
    };
}

#endif
