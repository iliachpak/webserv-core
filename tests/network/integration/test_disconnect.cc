#include <sys/socket.h>
#include <unistd.h>

#include "network/EventLoop.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

bool test_remove_connection_idempotent()
{
    int fds[2];
    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

    EventLoop loop(8);

    Connection* conn = new Connection(fds[0]);
    TEST_ASSERT(loop.addConnection(conn));
    TEST_ASSERT(loop.connectionCount() == 1);

    TEST_ASSERT(loop.removeConnection(fds[0]));
    TEST_ASSERT(loop.connectionCount() == 0);

    TEST_ASSERT(!loop.removeConnection(fds[0]));

    close(fds[1]);
    return true;
}
