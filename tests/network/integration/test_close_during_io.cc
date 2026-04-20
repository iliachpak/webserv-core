#include <sys/socket.h>
#include <unistd.h>

#include "network/EventLoop.hh"
#include "network/INetworkHandler.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

class CloseOnDataHandler : public INetworkHandler
{
    public:
        CloseOnDataHandler()
            : dataCalls(0),
              closeCalls(0)
        {
        }

        virtual void onConnectionData(Connection& conn)
        {
            ++dataCalls;
            conn.onDisconnect();
        }

        virtual void onConnectionClose(SocketFD)
        {
            ++closeCalls;
        }

    public:
        int dataCalls;
        int closeCalls;
};

bool test_handler_forced_close_during_read()
{
    int fds[2];
    TEST_ASSERT(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

    EventLoop loop(8);
    CloseOnDataHandler handler;
    loop.setHandler(&handler);

    Connection* conn = new Connection(fds[0]);
    TEST_ASSERT(loop.addConnection(conn));

    const char payload[] = "hello";
    TEST_ASSERT(write(fds[1], payload, 5) == 5);

    for (int i = 0; i < 20 && loop.connectionCount() != 0; ++i)
        loop.runOnce(10);

    TEST_ASSERT(handler.dataCalls >= 1);
    TEST_ASSERT(handler.closeCalls >= 1);
    TEST_ASSERT(loop.connectionCount() == 0);

    close(fds[1]);
    return true;
}
