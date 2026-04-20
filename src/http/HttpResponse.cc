#include "http/HttpResponse.hh"

#include <sstream>

namespace http
{
    HttpResponse::HttpResponse()
        : statusCode(200),
          reason("OK"),
          contentType("text/plain"),
          keepAlive(true)
    {
    }

    std::string HttpResponse::serialize() const
    {
        std::ostringstream out;
        out << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";
        out << "Content-Type: " << contentType << "\r\n";
        out << "Content-Length: " << body.size() << "\r\n";
        out << "Connection: " << (keepAlive ? "keep-alive" : "close") << "\r\n";

        for (std::map<std::string, std::string>::const_iterator it = headers.begin();
             it != headers.end();
             ++it)
        {
            out << it->first << ": " << it->second << "\r\n";
        }

        out << "\r\n";
        out << body;
        return out.str();
    }
}
