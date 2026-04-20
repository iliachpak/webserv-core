#include "network/Poller.hh"

#include <cerrno>
#include <cstring>

#ifdef __linux__
#include <unistd.h>
#endif

namespace net
{
#ifdef __linux__
    static uint32_t buildEpollMask(bool wantRead, bool wantWrite)
    {
        uint32_t mask = EPOLLERR | EPOLLHUP;

        if (wantRead)
            mask |= EPOLLIN;
        if (wantWrite)
            mask |= EPOLLOUT;

#ifdef EPOLLRDHUP
        mask |= EPOLLRDHUP;
#endif

        return mask;
    }

    static bool hasPeerHangup(uint32_t events)
    {
        if ((events & EPOLLHUP) != 0)
            return true;
#ifdef EPOLLRDHUP
        if ((events & EPOLLRDHUP) != 0)
            return true;
#endif
        return false;
    }

    Poller::Poller()
        : _epollFd(epoll_create(1))
    {
    }

    Poller::~Poller()
    {
        if (_epollFd != -1)
            close(_epollFd);
    }

    bool Poller::add(SocketFD fd, bool wantRead, bool wantWrite)
    {
        if (_epollFd == -1 || fd == INVALID_SOCKET_FD)
            return false;

        struct epoll_event ev;
        std::memset(&ev, 0, sizeof(ev));
        ev.data.fd = fd;
        ev.events = buildEpollMask(wantRead, wantWrite);

        return epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) == 0;
    }

    bool Poller::modify(SocketFD fd, bool wantRead, bool wantWrite)
    {
        if (_epollFd == -1 || fd == INVALID_SOCKET_FD)
            return false;

        struct epoll_event ev;
        std::memset(&ev, 0, sizeof(ev));
        ev.data.fd = fd;
        ev.events = buildEpollMask(wantRead, wantWrite);

        return epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == 0;
    }

    bool Poller::remove(SocketFD fd)
    {
        if (_epollFd == -1 || fd == INVALID_SOCKET_FD)
            return false;

        return epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == 0;
    }

    bool Poller::wait(std::vector<NetworkEvent>& events, int timeoutMs)
    {
        events.clear();

        if (_epollFd == -1)
            return false;

        struct epoll_event epollEvents[kMaxEvents];
        int ready = epoll_wait(_epollFd, epollEvents, kMaxEvents, timeoutMs);

        if (ready == 0)
            return true;

        if (ready < 0)
        {
            if (errno == EINTR)
                return true;
            return false;
        }

        for (int i = 0; i < ready; ++i)
        {
            SocketFD fd = epollEvents[i].data.fd;
            uint32_t mask = epollEvents[i].events;

            if ((mask & EPOLLERR) != 0)
            {
                events.push_back(NetworkEvent(fd, EVENT_ERROR));
                continue;
            }

            if (hasPeerHangup(mask))
            {
                events.push_back(NetworkEvent(fd, EVENT_HANGUP));
                continue;
            }

            if ((mask & EPOLLIN) != 0)
                events.push_back(NetworkEvent(fd, EVENT_READ));
            if ((mask & EPOLLOUT) != 0)
                events.push_back(NetworkEvent(fd, EVENT_WRITE));
        }

        return true;
    }
#else
    static short buildPollMask(bool wantRead, bool wantWrite)
    {
        short mask = POLLERR | POLLHUP;

        if (wantRead)
            mask |= POLLIN;
        if (wantWrite)
            mask |= POLLOUT;

        return mask;
    }

    static bool hasPeerHangup(short events)
    {
        return (events & POLLHUP) != 0;
    }

    Poller::Poller()
    {
    }

    Poller::~Poller()
    {
    }

    bool Poller::add(SocketFD fd, bool wantRead, bool wantWrite)
    {
        if (fd == INVALID_SOCKET_FD)
            return false;

        if (_indexByFd.find(fd) != _indexByFd.end())
            return false;

        struct pollfd descriptor;
        descriptor.fd = fd;
        descriptor.events = buildPollMask(wantRead, wantWrite);
        descriptor.revents = 0;

        _indexByFd[fd] = _pollFds.size();
        _pollFds.push_back(descriptor);
        return true;
    }

    bool Poller::modify(SocketFD fd, bool wantRead, bool wantWrite)
    {
        std::map<SocketFD, std::size_t>::iterator it = _indexByFd.find(fd);
        if (it == _indexByFd.end())
            return false;

        _pollFds[it->second].events = buildPollMask(wantRead, wantWrite);
        return true;
    }

    bool Poller::remove(SocketFD fd)
    {
        std::map<SocketFD, std::size_t>::iterator it = _indexByFd.find(fd);
        if (it == _indexByFd.end())
            return false;

        const std::size_t removeIndex = it->second;
        const std::size_t lastIndex = _pollFds.size() - 1;

        if (removeIndex != lastIndex)
        {
            _pollFds[removeIndex] = _pollFds[lastIndex];
            _indexByFd[_pollFds[removeIndex].fd] = removeIndex;
        }

        _pollFds.pop_back();
        _indexByFd.erase(it);
        return true;
    }

    bool Poller::wait(std::vector<NetworkEvent>& events, int timeoutMs)
    {
        events.clear();

        struct pollfd* fdArray = _pollFds.empty() ? NULL : &_pollFds[0];
        int ready = poll(fdArray,
                         static_cast<nfds_t>(_pollFds.size()),
                         timeoutMs);

        if (ready == 0)
            return true;

        if (ready < 0)
        {
            if (errno == EINTR)
                return true;
            return false;
        }

        for (std::size_t i = 0; i < _pollFds.size(); ++i)
        {
            short mask = _pollFds[i].revents;
            SocketFD fd = _pollFds[i].fd;

            if ((mask & POLLERR) != 0)
            {
                events.push_back(NetworkEvent(fd, EVENT_ERROR));
                continue;
            }

            if (hasPeerHangup(mask))
            {
                events.push_back(NetworkEvent(fd, EVENT_HANGUP));
                continue;
            }

            if ((mask & POLLIN) != 0)
                events.push_back(NetworkEvent(fd, EVENT_READ));
            if ((mask & POLLOUT) != 0)
                events.push_back(NetworkEvent(fd, EVENT_WRITE));
        }

        return true;
    }
#endif
}
