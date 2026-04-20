#ifndef HTTP_REQUEST_HH
#define HTTP_REQUEST_HH

#include <map>
#include <string>

namespace http
{
    struct HttpRequest
    {
        std::string method;
        std::string target;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        bool keepAlive;

        HttpRequest()
            : keepAlive(true)
        {
        }
    };
}

#endif
