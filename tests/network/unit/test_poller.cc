#include <unistd.h>
#include <sys/socket.h>

#include "network/Poller.hh"
#include "TestRunner.hh"
#include "TestAssertions.hh"

using namespace net;

/* helper: create socketpair */
static bool createSocketPair(int& a, int& b)
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
        return false;
    a = fds[0];
    b = fds[1];
    return true;
}

/* ================= BASIC ================= */

bool test_poller_add_invalid_fd()
{
    Poller poller;
    return !poller.add(-1, true, false);
}

bool test_poller_add_and_remove()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(poller.remove(a));

    close(a);
    close(b);
    return true;
}

bool test_poller_remove_unknown_fd()
{
    Poller poller;
    return !poller.remove(42);
}

bool test_poller_modify_unknown_fd()
{
    Poller poller;
    return !poller.modify(42, true, true);
}

/* ================= WAIT ================= */

bool test_poller_wait_empty()
{
    Poller poller;
    std::vector<NetworkEvent> events;

    TEST_ASSERT(poller.wait(events, 10));
    TEST_ASSERT(events.empty());
    return true;
}

bool test_poller_wait_timeout_zero()
{
    Poller poller;
    std::vector<NetworkEvent> events;

    TEST_ASSERT(poller.wait(events, 0));
    return true;
}

/* ================= EVENTS ================= */

bool test_poller_read_event()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));

    write(b, "x", 1);

    std::vector<NetworkEvent> events;
    TEST_ASSERT(poller.wait(events, 100));
    TEST_ASSERT(!events.empty());
    TEST_ASSERT(events[0].fd == a);
    TEST_ASSERT(events[0].type == EVENT_READ);

    close(a);
    close(b);
    return true;
}

bool test_poller_write_event()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, false, true));

    std::vector<NetworkEvent> events;
    TEST_ASSERT(poller.wait(events, 100));
    TEST_ASSERT(!events.empty());
    TEST_ASSERT(events[0].fd == a);
    TEST_ASSERT(events[0].type == EVENT_WRITE);

    close(a);
    close(b);
    return true;
}

bool test_poller_read_and_write_event()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, true));

    write(b, "x", 1);

    std::vector<NetworkEvent> events;
    TEST_ASSERT(poller.wait(events, 100));
    TEST_ASSERT(!events.empty());
    TEST_ASSERT(events[0].fd == a);
    TEST_ASSERT(events[0].type == EVENT_READ);

    TEST_ASSERT(events[1].fd == a);
    TEST_ASSERT(events[1].type == EVENT_WRITE);

    close(a);
    close(b);
    return true;
}

bool test_poller_hangup_event()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, false, true));

    close(b);

    std::vector<NetworkEvent> events;
    TEST_ASSERT(poller.wait(events, 100));
    TEST_ASSERT(!events.empty());
    TEST_ASSERT(events[0].fd == a);
    TEST_ASSERT(events[0].type == EVENT_HANGUP);

    close(a);
    return true;
}

/* ================= INVARIANTS ================= */

bool test_poller_double_add()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(!poller.add(a, true, false));

    close(a);
    close(b);
    return true;
}

bool test_poller_modify_flags()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(poller.modify(a, false, true));

    std::vector<NetworkEvent> events;
    TEST_ASSERT(poller.wait(events, 100));
    TEST_ASSERT(!events.empty());
    TEST_ASSERT(events[0].fd == a);
    TEST_ASSERT(events[0].type == EVENT_WRITE);

    close(a);
    close(b);
    return true;
}

bool test_poller_remove_then_wait()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(poller.remove(a));

    std::vector<NetworkEvent> events;
    TEST_ASSERT(poller.wait(events, 100));
    TEST_ASSERT(events.empty());

    close(a);
    close(b);
    return true;
}

bool test_poller_readd_after_remove()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(poller.remove(a));
    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(poller.remove(a));

    close(a);
    close(b);
    return true;
}

bool test_poller_modify_after_remove_fails()
{
    Poller poller;
    int a, b;
    TEST_ASSERT(createSocketPair(a, b));

    TEST_ASSERT(poller.add(a, true, false));
    TEST_ASSERT(poller.remove(a));
    TEST_ASSERT(!poller.modify(a, true, true));

    close(a);
    close(b);
    return true;
}

bool test_poller_wait_clears_previous_events()
{
    Poller poller;
    std::vector<NetworkEvent> events;

    events.push_back(NetworkEvent(123, EVENT_READ));
    TEST_ASSERT(poller.wait(events, 0));
    TEST_ASSERT(events.empty());
    return true;
}
