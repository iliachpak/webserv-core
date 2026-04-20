#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <vector>
#include <iostream>

#include "TestRunner.hh"
#include "TestAssertions.hh"
#include "network/SocketUtils.hh"
#include "network/Connection.hh"
#include "network/EventLoop.hh"

using namespace net;

static bool createClientSocket(const std::string& host, unsigned short port, SocketFD& clientFd)
{
    clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd < 0) return false;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    return connect(clientFd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

bool test_accept_connection()
{
    EventLoop loop(64);
    unsigned short port = 0;

    const unsigned short basePort = static_cast<unsigned short>(19000 + (getpid() % 2000));
    for (int i = 0; i < 40; ++i)
    {
        unsigned short candidate = static_cast<unsigned short>(basePort + i);
        if (loop.addServer("", candidate, 10))
        {
            port = candidate;
            break;
        }
    }

    if (port == 0)
    {
        std::cerr << "[SKIP] accept integration test: no bindable port in this environment" << std::endl;
        return true;
    }

    // lancer le client
    SocketFD clientFd;
    TEST_ASSERT(createClientSocket("127.0.0.1", port, clientFd));

    std::vector<NetworkEvent> events;

    // faire tourner l’event loop quelques tours
    int elapsed = 0;
    while (loop.connectionCount() < 1 && elapsed < 500) {
        loop.runOnce(50, events);
        elapsed += 50;
    }

    // le serveur doit avoir accepté la connexion
    TEST_ASSERT(loop.connectionCount() == 1);

    close(clientFd);
    return true;
}
