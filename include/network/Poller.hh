#ifndef EPOLL_POLLER_HH
#define EPOLL_POLLER_HH

#include <map>
#include <vector>

#ifdef __linux__
#include <sys/epoll.h>
#else
#include <poll.h>
#endif

#include "network/NetworkEvent.hh"
#include "network/NetworkTypes.hh"

namespace net
{
    class Poller
    {
    public:
        Poller();
        ~Poller();

        bool add(SocketFD fd, bool wantRead, bool wantWrite);
        bool modify(SocketFD fd, bool wantRead, bool wantWrite);
        bool remove(SocketFD fd);

        bool wait(std::vector<NetworkEvent>& events, int timeoutMs);

    private:
        Poller(const Poller&);
        Poller& operator=(const Poller&);

    private:
        enum
        {
            kMaxEvents = 64
        };

#ifdef __linux__
        int _epollFd;
#else
        std::vector<struct pollfd> _pollFds;
        std::map<SocketFD, std::size_t> _indexByFd;
#endif
    };
}

#endif
