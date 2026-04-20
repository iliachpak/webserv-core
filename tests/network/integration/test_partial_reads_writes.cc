#include <sys/socket.h>
#include <unistd.h>

#include "network/EventLoop.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

bool test_partial_reads_across_multiple_ticks()
{
    int fds[2];
    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

    EventLoop loop(8);
    Connection* conn = new Connection(fds[0]);
    TEST_ASSERT(loop.addConnection(conn));

    TEST_ASSERT(write(fds[1], "abc", 3) == 3);

    for (int i = 0; i < 20 && conn->getReadBuffer().size() < 3; ++i)
        loop.runOnce(10);

    TEST_ASSERT(conn->getReadBuffer().size() >= 3);

    TEST_ASSERT(write(fds[1], "defgh", 5) == 5);

    for (int i = 0; i < 20 && conn->getReadBuffer().size() < 8; ++i)
        loop.runOnce(10);

    TEST_ASSERT(conn->getReadBuffer().size() >= 8);

    TEST_ASSERT(loop.removeConnection(fds[0]));
    close(fds[1]);
    return true;
}
