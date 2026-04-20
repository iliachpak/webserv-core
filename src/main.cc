#include <csignal>
#include <cstdlib>
#include <iostream>

#include "http/HttpServerHandler.hh"
#include "network/EventLoop.hh"
#include "network/ListenerConfig.hh"

namespace
{
    volatile sig_atomic_t g_keepRunning = 1;

    void stopSignalHandler(int)
    {
        g_keepRunning = 0;
    }

    unsigned short parsePort(const char* arg)
    {
        char* endPtr = NULL;
        long value = std::strtol(arg, &endPtr, 10);

        if (endPtr == arg || *endPtr != '\0' || value <= 0 || value > 65535)
            return 0;

        return static_cast<unsigned short>(value);
    }

    std::size_t parseMaxClients(const char* arg)
    {
        char* endPtr = NULL;
        long value = std::strtol(arg, &endPtr, 10);

        if (endPtr == arg || *endPtr != '\0' || value <= 0)
            return 0;

        return static_cast<std::size_t>(value);
    }
}

int main(int argc, char** argv)
{
    unsigned short port = 8080;
    std::size_t maxClients = 1024;

    if (argc > 1)
    {
        port = parsePort(argv[1]);
        if (port == 0)
        {
            std::cerr << "Invalid port. Usage: ./webserv [port] [max_clients]" << std::endl;
            return 1;
        }
    }

    if (argc > 2)
    {
        maxClients = parseMaxClients(argv[2]);
        if (maxClients == 0)
        {
            std::cerr << "Invalid max_clients. Usage: ./webserv [port] [max_clients]" << std::endl;
            return 1;
        }
    }

    std::signal(SIGINT, stopSignalHandler);
    std::signal(SIGTERM, stopSignalHandler);

    net::EventLoop loop(maxClients);
    http::HttpServerHandler httpHandler;

    loop.setHandler(&httpHandler);

    net::ListenerConfig listener("0.0.0.0", port, 128);
    if (!loop.addServer(listener))
    {
        std::cerr << "Failed to bind listener on port " << port << std::endl;
        return 1;
    }

    std::cout << "webserv minimal HTTP listening on 0.0.0.0:" << port
              << " (max_clients=" << maxClients << ")" << std::endl;
    std::cout << "Routes: GET /, GET /health, POST /echo, DELETE /resource" << std::endl;
    std::cout << "Press Ctrl+C to stop." << std::endl;

    while (g_keepRunning)
        loop.runOnce(250);

    loop.stop();
    std::cout << "Server stopped." << std::endl;
    return 0;
}
