#ifndef HTTP_RESPONSE_HH
#define HTTP_RESPONSE_HH

#include <map>
#include <string>

namespace http
{
    struct HttpResponse
    {
        int statusCode;
        std::string reason;
        std::string contentType;
        std::string body;
        bool keepAlive;
        std::map<std::string, std::string> headers;

        HttpResponse();
        std::string serialize() const;
    };
}

#endif
