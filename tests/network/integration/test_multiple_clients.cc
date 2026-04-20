#include <sys/socket.h>
#include <unistd.h>

#include "network/EventLoop.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

bool test_multiple_manual_connections_receive_data()
{
    int c1[2];
    int c2[2];

    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, c1) == 0);
    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, c2) == 0);

    EventLoop loop(16);

    Connection* conn1 = new Connection(c1[0]);
    Connection* conn2 = new Connection(c2[0]);

    TEST_ASSERT(loop.addConnection(conn1));
    TEST_ASSERT(loop.addConnection(conn2));
    TEST_ASSERT(loop.connectionCount() == 2);

    TEST_ASSERT(write(c1[1], "A", 1) == 1);
    TEST_ASSERT(write(c2[1], "B", 1) == 1);

    for (int i = 0; i < 20; ++i)
    {
        loop.runOnce(10);
        if (!conn1->getReadBuffer().empty() && !conn2->getReadBuffer().empty())
            break;
    }

    TEST_ASSERT(!conn1->getReadBuffer().empty());
    TEST_ASSERT(!conn2->getReadBuffer().empty());

    TEST_ASSERT(loop.removeConnection(c1[0]));
    TEST_ASSERT(loop.removeConnection(c2[0]));

    close(c1[1]);
    close(c2[1]);
    return true;
}
