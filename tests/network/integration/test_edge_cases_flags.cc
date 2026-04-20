#include <sys/socket.h>
#include <unistd.h>

#include "network/EventLoop.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

bool test_event_loop_rejects_null_connection()
{
    EventLoop loop(8);
    return !loop.addConnection(NULL);
}

bool test_event_loop_without_handler_still_reads()
{
    int fds[2];
    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

    EventLoop loop(8);
    Connection* conn = new Connection(fds[0]);
    TEST_ASSERT(loop.addConnection(conn));

    const char payload[] = "abc";
    TEST_ASSERT(write(fds[1], payload, 3) == 3);

    for (int i = 0; i < 20 && conn->getReadBuffer().empty(); ++i)
        loop.runOnce(10);

    TEST_ASSERT(!conn->getReadBuffer().empty());
    TEST_ASSERT(loop.connectionCount() == 1);

    TEST_ASSERT(loop.removeConnection(fds[0]));
    close(fds[1]);
    return true;
}
