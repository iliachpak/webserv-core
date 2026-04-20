#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include "network/Connection.hh"
#include "TestRunner.hh"
#include "TestAssertions.hh"

using namespace net;

static bool setNonBlockingFd(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool test_connection_basic()
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        return false;

    Connection conn(fds[0]);

    TEST_ASSERT(conn.getFd() == fds[0]);
    TEST_ASSERT(!conn.isClosed());

    close(fds[1]); // l'autre extrémité
    return true;
}

bool test_connection_write_buffer()
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        return false;

    Connection conn(fds[0]);

    TEST_ASSERT(!conn.wantsWrite());

    const char msg[] = "hello";
    conn.appendToWriteBuffer(msg, sizeof(msg));

    TEST_ASSERT(conn.wantsWrite());

    close(fds[1]);
    return true;
}

bool test_connection_read_buffer()
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        return false;

    Connection conn(fds[0]);

    const char msg[] = "ping";
    write(fds[1], msg, sizeof(msg));

    conn.onReadable();

    TEST_ASSERT(!conn.getReadBuffer().empty());

    conn.clearReadBuffer();
    TEST_ASSERT(conn.getReadBuffer().empty());

    close(fds[1]);
    return true;
}

bool test_connection_disconnect()
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        return false;

    Connection conn(fds[0]);

    conn.onDisconnect();

    TEST_ASSERT(conn.isClosed());

    close(fds[1]);
    return true;
}

bool test_connection_wants_flags()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);

    TEST_ASSERT(conn.wantsRead());     // par défaut
    TEST_ASSERT(!conn.wantsWrite());   // pas de buffer write

    const char msg[] = "x";
    conn.appendToWriteBuffer(msg, 1);

    TEST_ASSERT(conn.wantsWrite());

    close(fds[1]);
    return true;
}

bool test_connection_onWritable_clears_buffer()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);

    const char msg[] = "hello";
    conn.appendToWriteBuffer(msg, sizeof(msg));

    conn.onWritable();

    TEST_ASSERT(!conn.wantsWrite());

    close(fds[1]);
    return true;
}

bool test_connection_partial_read()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);

    write(fds[1], "abc", 3);
    write(fds[1], "def", 3);

    conn.onReadable();

    TEST_ASSERT(conn.getReadBuffer().size() >= 3);

    close(fds[1]);
    return true;
}

bool test_connection_double_disconnect()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);

    conn.onDisconnect();
    conn.onDisconnect(); // ne doit pas crash

    TEST_ASSERT(conn.isClosed());

    close(fds[1]);
    return true;
}

bool test_connection_no_io_after_close()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);
    conn.onDisconnect();

    const char msg[] = "x";
    conn.appendToWriteBuffer(msg, 1);
    conn.onWritable();
    conn.onReadable();

    TEST_ASSERT(conn.getReadBuffer().empty());

    close(fds[1]);
    return true;
}

bool test_connection_append_invalid_inputs()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);

    conn.appendToWriteBuffer(NULL, 5);
    TEST_ASSERT(conn.pendingWriteBytes() == 0);

    const char data[] = "abc";
    conn.appendToWriteBuffer(data, 0);
    TEST_ASSERT(conn.pendingWriteBytes() == 0);

    close(fds[1]);
    return true;
}

bool test_connection_pending_write_tracking()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0]);

    const char msgA[] = "abc";
    const char msgB[] = "de";

    conn.appendToWriteBuffer(msgA, 3);
    conn.appendToWriteBuffer(msgB, 2);

    TEST_ASSERT(conn.pendingWriteBytes() == 5);
    TEST_ASSERT(conn.wantsWrite());

    conn.onWritable();

    TEST_ASSERT(conn.pendingWriteBytes() == 0);
    TEST_ASSERT(!conn.wantsWrite());

    close(fds[1]);
    return true;
}

bool test_connection_read_limit_disconnect()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    Connection conn(fds[0], 4);
    const char payload[] = "12345";
    write(fds[1], payload, 5);

    conn.onReadable();

    TEST_ASSERT(conn.isClosed());
    close(fds[1]);
    return true;
}

bool test_connection_nonblocking_empty_read()
{
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    TEST_ASSERT(setNonBlockingFd(fds[0]));
    Connection conn(fds[0]);

    conn.onReadable();

    TEST_ASSERT(!conn.isClosed());
    TEST_ASSERT(conn.getReadBuffer().empty());

    close(fds[1]);
    return true;
}
