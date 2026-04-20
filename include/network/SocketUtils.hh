#ifndef SOCKET_UTILS_HH
#define SOCKET_UTILS_HH

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "network/NetworkTypes.hh"

namespace net
{
    namespace SocketUtils
    {
        inline bool setNonBlocking(SocketFD fd)
        {
            if (fd == INVALID_SOCKET_FD)
                return false;

            int flags = fcntl(fd, F_GETFL, 0);
            if (flags == -1)
                return false;

            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
                return false;

            return true;
        }

        inline void closeSocket(SocketFD fd)
        {
            if (fd != INVALID_SOCKET_FD)
                close(fd);
        }

        inline SocketFD createServerSocket(const std::string& host,
                                           unsigned short port,
                                           int backlog)
        {
            struct addrinfo hints;
            struct addrinfo* result = NULL;

            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;

            char portString[6];
            std::snprintf(portString, sizeof(portString), "%u", port);

            const char* hostPtr = host.empty() ? NULL : host.c_str();
            if (getaddrinfo(hostPtr, portString, &hints, &result) != 0)
                return INVALID_SOCKET_FD;

            SocketFD serverFd = INVALID_SOCKET_FD;

            for (struct addrinfo* it = result; it != NULL; it = it->ai_next)
            {
                serverFd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
                if (serverFd == INVALID_SOCKET_FD)
                    continue;

                int opt = 1;
                if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
                {
                    closeSocket(serverFd);
                    serverFd = INVALID_SOCKET_FD;
                    continue;
                }

#ifdef SO_REUSEPORT
                setsockopt(serverFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#endif

                if (!setNonBlocking(serverFd))
                {
                    closeSocket(serverFd);
                    serverFd = INVALID_SOCKET_FD;
                    continue;
                }

                if (bind(serverFd, it->ai_addr, it->ai_addrlen) == -1)
                {
                    closeSocket(serverFd);
                    serverFd = INVALID_SOCKET_FD;
                    continue;
                }

                if (listen(serverFd, backlog) == -1)
                {
                    closeSocket(serverFd);
                    serverFd = INVALID_SOCKET_FD;
                    continue;
                }

                break;
            }

            freeaddrinfo(result);
            return serverFd;
        }
    }
}

#endif
