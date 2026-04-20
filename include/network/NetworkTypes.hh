#ifndef NETWORK_TYPES_HH
#define NETWORK_TYPES_HH

#include <cstddef>

namespace net
{
    typedef int SocketFD;
    static const SocketFD INVALID_SOCKET_FD = -1;

    enum IOEvent
    {
        EVENT_READ = 0,
        EVENT_WRITE,
        EVENT_ERROR,
        EVENT_HANGUP
    };
}

#endif
