#include <sys/socket.h>
#include <unistd.h>

#include "network/ConnectionRegistry.hh"
#include "network/Connection.hh"
#include "TestAssertions.hh"

using namespace net;

static bool createSocketPair(int& a, int& b)
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
        return false;
    a = fds[0];
    b = fds[1];
    return true;
}

bool test_add_and_get()
{
    ConnectionRegistry registry;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    Connection* c1 = new Connection(a);

    TEST_ASSERT(registry.add(c1));

    Connection* found = registry.get(a);
    TEST_ASSERT(found == c1);

    close(b);
    return true;
}

bool test_add_duplicate_fd()
{
    ConnectionRegistry registry;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    Connection* c1 = new Connection(a);
    Connection* c2 = new Connection(a);

    TEST_ASSERT(registry.add(c1));
    TEST_ASSERT(!registry.add(c2));

    delete c2;
    close(b);
    return true;
}

bool test_remove_connection()
{
    ConnectionRegistry registry;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(registry.add(new Connection(a)));
    TEST_ASSERT(registry.remove(a));
    TEST_ASSERT(registry.get(a) == NULL);

    close(b);
    return true;
}

bool test_remove_unknown_fd()
{
    ConnectionRegistry registry;
    return !registry.remove(999);
}

bool test_clear_registry()
{
    ConnectionRegistry registry;
    int a1, b1, a2, b2;
    TEST_ASSERT(createSocketPair(a1, b1));
    TEST_ASSERT(createSocketPair(a2, b2));

    TEST_ASSERT(registry.add(new Connection(a1)));
    TEST_ASSERT(registry.add(new Connection(a2)));

    registry.clear();

    TEST_ASSERT(registry.get(a1) == NULL);
    TEST_ASSERT(registry.get(a2) == NULL);
    TEST_ASSERT(registry.size() == 0);

    close(b1);
    close(b2);
    return true;
}

bool test_add_null_connection()
{
    ConnectionRegistry registry;
    return !registry.add(NULL);
}

bool test_registry_contains_size_consistency()
{
    ConnectionRegistry registry;
    int a1, b1, a2, b2;
    TEST_ASSERT(createSocketPair(a1, b1));
    TEST_ASSERT(createSocketPair(a2, b2));

    TEST_ASSERT(registry.size() == 0);
    TEST_ASSERT(!registry.contains(a1));

    TEST_ASSERT(registry.add(new Connection(a1)));
    TEST_ASSERT(registry.add(new Connection(a2)));

    TEST_ASSERT(registry.size() == 2);
    TEST_ASSERT(registry.contains(a1));
    TEST_ASSERT(registry.contains(a2));

    TEST_ASSERT(registry.remove(a1));
    TEST_ASSERT(registry.size() == 1);
    TEST_ASSERT(!registry.contains(a1));
    TEST_ASSERT(registry.contains(a2));

    close(b1);
    close(b2);
    return true;
}

bool test_remove_twice_same_fd()
{
    ConnectionRegistry registry;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(registry.add(new Connection(a)));
    TEST_ASSERT(registry.remove(a));
    TEST_ASSERT(!registry.remove(a));

    close(b);
    return true;
}

bool test_clear_calls_connection_destructors()
{
    ConnectionRegistry registry;
    int a1, b1, a2, b2;
    TEST_ASSERT(createSocketPair(a1, b1));
    TEST_ASSERT(createSocketPair(a2, b2));

    TEST_ASSERT(registry.add(new Connection(a1)));
    TEST_ASSERT(registry.add(new Connection(a2)));

    registry.clear();
    TEST_ASSERT(registry.size() == 0);

    close(b1);
    close(b2);
    return true;
}
