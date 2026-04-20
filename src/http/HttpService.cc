#include "http/HttpService.hh"

#include <sstream>

namespace http
{
    static HttpResponse makeResponse(int code,
                                     const std::string& reason,
                                     const std::string& body,
                                     bool keepAlive)
    {
        HttpResponse response;
        response.statusCode = code;
        response.reason = reason;
        response.body = body;
        response.keepAlive = keepAlive;
        return response;
    }

    HttpResponse HttpService::buildResponse(const HttpRequest& request) const
    {
        if (request.version != "HTTP/1.1")
            return makeResponse(505,
                                "HTTP Version Not Supported",
                                "Only HTTP/1.1 is supported\n",
                                false);

        if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
            return makeResponse(405,
                                "Method Not Allowed",
                                "Allowed methods: GET, POST, DELETE\n",
                                request.keepAlive);

        if (request.method == "GET" && request.target == "/health")
            return makeResponse(200, "OK", "OK\n", request.keepAlive);

        if (request.method == "GET" && request.target == "/")
            return makeResponse(200,
                                "OK",
                                "webserv-core minimal HTTP layer\n",
                                request.keepAlive);

        if (request.method == "POST" && request.target == "/echo")
        {
            HttpResponse response = makeResponse(200,
                                                 "OK",
                                                 request.body,
                                                 request.keepAlive);
            response.headers["X-Body-Size"] = request.body.empty() ? "0" : "1";
            if (!request.body.empty())
            {
                std::ostringstream size;
                size << request.body.size();
                response.headers["X-Body-Size"] = size.str();
            }
            return response;
        }

        if (request.method == "DELETE" && request.target == "/resource")
            return makeResponse(204, "No Content", "", request.keepAlive);

        return makeResponse(404,
                            "Not Found",
                            "Route not found\n",
                            request.keepAlive);
    }
}
