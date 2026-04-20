#include <iostream>
#include "TestRunner.hh"

// include tous les tests unitaires
bool test_add_and_get();
bool test_add_duplicate_fd();
bool test_remove_connection();
bool test_remove_unknown_fd();
bool test_clear_registry();
bool test_add_null_connection();
bool test_registry_contains_size_consistency();
bool test_remove_twice_same_fd();
bool test_clear_calls_connection_destructors();

bool test_connection_basic();
bool test_connection_write_buffer();
bool test_connection_read_buffer();
bool test_connection_disconnect();
bool test_connection_wants_flags();
bool test_connection_onWritable_clears_buffer();
bool test_connection_partial_read();
bool test_connection_double_disconnect();
bool test_connection_no_io_after_close();
bool test_connection_append_invalid_inputs();
bool test_connection_pending_write_tracking();
bool test_connection_read_limit_disconnect();
bool test_connection_nonblocking_empty_read();

bool test_poller_add_invalid_fd();
bool test_poller_add_and_remove();
bool test_poller_remove_unknown_fd();
bool test_poller_modify_unknown_fd();
bool test_poller_wait_empty();
bool test_poller_wait_timeout_zero();
bool test_poller_read_event();
bool test_poller_write_event();
bool test_poller_read_and_write_event();
bool test_poller_hangup_event();
bool test_poller_double_add();
bool test_poller_modify_flags();
bool test_poller_remove_then_wait();
bool test_poller_readd_after_remove();
bool test_poller_modify_after_remove_fails();
bool test_poller_wait_clears_previous_events();

// include tous les tests d'integrations
bool test_accept_connection();
bool test_remove_connection_idempotent();
bool test_manual_connection_read_write_cycle();
bool test_handler_forced_close_during_read();
bool test_event_loop_rejects_null_connection();
bool test_event_loop_without_handler_still_reads();
bool test_multiple_manual_connections_receive_data();
bool test_partial_reads_across_multiple_ticks();

int main()
{
    test::TestRunner runner;

    // Enregistrement des tests unitaires
    runner.addSection("Unit tests");

    runner.addTest("[network][unit][registry] add and get", test_add_and_get);
    runner.addTest("[network][unit][registry] duplicate fd", test_add_duplicate_fd);
    runner.addTest("[network][unit][registry] remove connection", test_remove_connection);
    runner.addTest("[network][unit][registry] remove unknown fd", test_remove_unknown_fd);
    runner.addTest("[network][unit][registry] clear registry", test_clear_registry);
    runner.addTest("[network][unit][registry] add null connection", test_add_null_connection);
    runner.addTest("[network][unit][registry] contains + size consistency", test_registry_contains_size_consistency);
    runner.addTest("[network][unit][registry] remove twice same fd", test_remove_twice_same_fd);
    runner.addTest("[network][unit][registry] clear destroys owned connections", test_clear_calls_connection_destructors);
    
    runner.addTest("[network][unit][connnection] basic", test_connection_basic);
    runner.addTest("[network][unit][connnection] write buffer", test_connection_write_buffer);
    runner.addTest("[network][unit][connnection] read buffer", test_connection_read_buffer);
    runner.addTest("[network][unit][connnection] disconnect", test_connection_disconnect);
    runner.addTest("[network][unit][connnection] wants flags", test_connection_wants_flags);
    runner.addTest("[network][unit][connnection] onWritable clears buffer", test_connection_onWritable_clears_buffer);
    runner.addTest("[network][unit][connnection] partial read", test_connection_partial_read);
    runner.addTest("[network][unit][connnection] double disconnect", test_connection_double_disconnect);
    runner.addTest("[network][unit][connnection] no I/O after close", test_connection_no_io_after_close);
    runner.addTest("[network][unit][connnection] append invalid inputs", test_connection_append_invalid_inputs);
    runner.addTest("[network][unit][connnection] pending write tracking", test_connection_pending_write_tracking);
    runner.addTest("[network][unit][connnection] read limit disconnect", test_connection_read_limit_disconnect);
    runner.addTest("[network][unit][connnection] nonblocking empty read", test_connection_nonblocking_empty_read);

    runner.addTest("[network][unit][poller] add invalid fd", test_poller_add_invalid_fd);
    runner.addTest("[network][unit][poller] add and remove", test_poller_add_and_remove);
    runner.addTest("[network][unit][poller] remove unknown fd", test_poller_remove_unknown_fd);
    runner.addTest("[network][unit][poller] modify unknown fd", test_poller_modify_unknown_fd);

    runner.addTest("[network][unit][poller] wait empty", test_poller_wait_empty);
    runner.addTest("[network][unit][poller] wait timeout zero", test_poller_wait_timeout_zero);

    runner.addTest("[network][unit][poller] read event", test_poller_read_event);
    runner.addTest("[network][unit][poller] write event", test_poller_write_event);
    runner.addTest("[network][unit][poller] read and write events", test_poller_read_and_write_event);
    runner.addTest("[network][unit][poller] Hang up event", test_poller_hangup_event);

    runner.addTest("[network][unit][poller] double add", test_poller_double_add);
    runner.addTest("[network][unit][poller] modify flags", test_poller_modify_flags);
    runner.addTest("[network][unit][poller] remove then wait", test_poller_remove_then_wait);
    runner.addTest("[network][unit][poller] re-add after remove", test_poller_readd_after_remove);
    runner.addTest("[network][unit][poller] modify after remove fails", test_poller_modify_after_remove_fails);
    runner.addTest("[network][unit][poller] wait clears previous events", test_poller_wait_clears_previous_events);

    runner.addSection("integration tests");

    runner.addTest("[network][integration][accept] accept connection", test_accept_connection);
    runner.addTest("[network][integration][disconnect] remove connection idempotent", test_remove_connection_idempotent);
    runner.addTest("[network][integration][rw] manual read/write cycle", test_manual_connection_read_write_cycle);
    runner.addTest("[network][integration][close] forced close during read", test_handler_forced_close_during_read);
    runner.addTest("[network][integration][edge] rejects null connection", test_event_loop_rejects_null_connection);
    runner.addTest("[network][integration][edge] no handler still reads", test_event_loop_without_handler_still_reads);
    runner.addTest("[network][integration][multi] multiple manual connections", test_multiple_manual_connections_receive_data);
    runner.addTest("[network][integration][partial] reads across ticks", test_partial_reads_across_multiple_ticks);

    return runner.run();
}
