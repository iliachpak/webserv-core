#ifndef HTTP_PARSER_HH
#define HTTP_PARSER_HH

#include <cstddef>
#include <string>

#include "http/HttpRequest.hh"

namespace http
{
    class HttpParser
    {
    public:
        enum ParseResult
        {
            PARSE_INCOMPLETE,
            PARSE_OK,
            PARSE_BAD_REQUEST
        };

        ParseResult parse(const std::string& input,
                          HttpRequest& request,
                          std::size_t& consumedBytes) const;
    };
}

#endif
