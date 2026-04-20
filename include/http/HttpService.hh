#ifndef HTTP_SERVICE_HH
#define HTTP_SERVICE_HH

#include "http/HttpRequest.hh"
#include "http/HttpResponse.hh"

namespace http
{
    class HttpService
    {
    public:
        HttpResponse buildResponse(const HttpRequest& request) const;
    };
}

#endif
