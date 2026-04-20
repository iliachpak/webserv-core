#include "TestRunner.hh"

bool test_parser_complete_get();
bool test_parser_incomplete_headers();
bool test_parser_content_length_body();
bool test_parser_bad_header_line();
bool test_parser_connection_close();

bool test_service_method_not_allowed();
bool test_service_route_not_found();
bool test_service_post_echo();
bool test_service_delete_resource();
bool test_service_version_not_supported();

int main()
{
    test::TestRunner runner;

    runner.addSection("HTTP parser tests");
    runner.addTest("[http][parser] complete GET", test_parser_complete_get);
    runner.addTest("[http][parser] incomplete headers", test_parser_incomplete_headers);
    runner.addTest("[http][parser] content-length body", test_parser_content_length_body);
    runner.addTest("[http][parser] bad header line", test_parser_bad_header_line);
    runner.addTest("[http][parser] connection close flag", test_parser_connection_close);

    runner.addSection("HTTP service tests");
    runner.addTest("[http][service] method not allowed", test_service_method_not_allowed);
    runner.addTest("[http][service] route not found", test_service_route_not_found);
    runner.addTest("[http][service] post echo", test_service_post_echo);
    runner.addTest("[http][service] delete resource", test_service_delete_resource);
    runner.addTest("[http][service] version not supported", test_service_version_not_supported);

    return runner.run();
}
