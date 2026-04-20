#ifndef LISTENER_CONFIG_HH
#define LISTENER_CONFIG_HH

#include <string>

namespace net
{
    struct ListenerConfig
    {
        std::string    host;
        unsigned short port;
        int            backlog;

        ListenerConfig()
            : host(""),
              port(8080),
              backlog(128)
        {
        }

        ListenerConfig(const std::string& listenerHost,
                       unsigned short listenerPort,
                       int listenerBacklog)
            : host(listenerHost),
              port(listenerPort),
              backlog(listenerBacklog)
        {
        }
    };
}

#endif
