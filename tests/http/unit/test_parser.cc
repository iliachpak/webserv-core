#include <string>

#include "http/HttpParser.hh"
#include "TestAssertions.hh"

bool test_parser_complete_get()
{
    http::HttpParser parser;
    http::HttpRequest req;

    const std::string raw = "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n";

    std::size_t consumed = 0;
    http::HttpParser::ParseResult result = parser.parse(raw, req, consumed);

    TEST_ASSERT(result == http::HttpParser::PARSE_OK);
    TEST_ASSERT(consumed == raw.size());
    TEST_ASSERT(req.method == "GET");
    TEST_ASSERT(req.target == "/health");
    TEST_ASSERT(req.version == "HTTP/1.1");
    TEST_ASSERT(req.body.empty());
    TEST_ASSERT(req.keepAlive);
    return true;
}

bool test_parser_incomplete_headers()
{
    http::HttpParser parser;
    http::HttpRequest req;

    const std::string raw = "GET / HTTP/1.1\r\nHost: localhost\r\n";

    std::size_t consumed = 0;
    http::HttpParser::ParseResult result = parser.parse(raw, req, consumed);

    TEST_ASSERT(result == http::HttpParser::PARSE_INCOMPLETE);
    TEST_ASSERT(consumed == 0);
    return true;
}

bool test_parser_content_length_body()
{
    http::HttpParser parser;
    http::HttpRequest req;

    const std::string raw = "POST /echo HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nhello";

    std::size_t consumed = 0;
    http::HttpParser::ParseResult result = parser.parse(raw, req, consumed);

    TEST_ASSERT(result == http::HttpParser::PARSE_OK);
    TEST_ASSERT(consumed == raw.size());
    TEST_ASSERT(req.method == "POST");
    TEST_ASSERT(req.target == "/echo");
    TEST_ASSERT(req.body == "hello");
    return true;
}

bool test_parser_bad_header_line()
{
    http::HttpParser parser;
    http::HttpRequest req;

    const std::string raw = "GET / HTTP/1.1\r\nHost localhost\r\n\r\n";

    std::size_t consumed = 0;
    http::HttpParser::ParseResult result = parser.parse(raw, req, consumed);

    TEST_ASSERT(result == http::HttpParser::PARSE_BAD_REQUEST);
    return true;
}

bool test_parser_connection_close()
{
    http::HttpParser parser;
    http::HttpRequest req;

    const std::string raw = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";

    std::size_t consumed = 0;
    http::HttpParser::ParseResult result = parser.parse(raw, req, consumed);

    TEST_ASSERT(result == http::HttpParser::PARSE_OK);
    TEST_ASSERT(!req.keepAlive);
    return true;
}
