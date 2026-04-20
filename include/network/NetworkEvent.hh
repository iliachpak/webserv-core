#ifndef NETWORK_EVENT_HH
#define NETWORK_EVENT_HH

#include "network/NetworkTypes.hh"

namespace net
{
    struct NetworkEvent
    {
        SocketFD fd;
        IOEvent  type;

        NetworkEvent()
            : fd(INVALID_SOCKET_FD),
              type(EVENT_ERROR)
        {
        }

        NetworkEvent(SocketFD socketFd, IOEvent eventType)
            : fd(socketFd),
              type(eventType)
        {
        }
    };
}

#endif
