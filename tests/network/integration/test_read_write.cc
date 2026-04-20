#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>

#include "network/EventLoop.hh"
#include "network/INetworkHandler.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

static bool setNonBlockingFd(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

class CountingHandler : public INetworkHandler
{
    public:
        CountingHandler()
            : dataCalls(0),
              writableCalls(0),
              closeCalls(0)
        {
        }

        virtual void onConnectionData(Connection&)
        {
            ++dataCalls;
        }

        virtual void onConnectionWritable(Connection&)
        {
            ++writableCalls;
        }

        virtual void onConnectionClose(SocketFD)
        {
            ++closeCalls;
        }

    public:
        int dataCalls;
        int writableCalls;
        int closeCalls;
};

bool test_manual_connection_read_write_cycle()
{
    int fds[2];
    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);
    TEST_ASSERT(setNonBlockingFd(fds[1]));

    EventLoop loop(8);
    CountingHandler handler;
    loop.setHandler(&handler);

    Connection* conn = new Connection(fds[0]);
    const char outbound[] = "pong";
    conn->appendToWriteBuffer(outbound, 4);

    TEST_ASSERT(loop.addConnection(conn));

    for (int i = 0; i < 20 && conn->pendingWriteBytes() > 0; ++i)
        loop.runOnce(10);

    TEST_ASSERT(conn->pendingWriteBytes() == 0);

    char recvBuf[16];
    ssize_t received = read(fds[1], recvBuf, sizeof(recvBuf));
    TEST_ASSERT(received == 4);

    const char inbound[] = "ping";
    TEST_ASSERT(write(fds[1], inbound, 4) == 4);

    for (int i = 0; i < 20 && handler.dataCalls == 0; ++i)
        loop.runOnce(10);

    TEST_ASSERT(handler.dataCalls >= 1);
    TEST_ASSERT(!conn->getReadBuffer().empty());

    close(fds[1]);

    for (int i = 0; i < 20 && handler.closeCalls == 0; ++i)
        loop.runOnce(10);

    TEST_ASSERT(handler.closeCalls >= 1);
    TEST_ASSERT(loop.connectionCount() == 0);

    return true;
}
