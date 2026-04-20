#include "http/HttpService.hh"
#include "TestAssertions.hh"

static http::HttpRequest makeRequest(const std::string& method,
                                     const std::string& target,
                                     const std::string& version)
{
    http::HttpRequest req;
    req.method = method;
    req.target = target;
    req.version = version;
    req.keepAlive = true;
    return req;
}

bool test_service_method_not_allowed()
{
    http::HttpService service;
    http::HttpRequest req = makeRequest("PUT", "/", "HTTP/1.1");

    http::HttpResponse resp = service.buildResponse(req);
    TEST_ASSERT(resp.statusCode == 405);
    return true;
}

bool test_service_route_not_found()
{
    http::HttpService service;
    http::HttpRequest req = makeRequest("GET", "/missing", "HTTP/1.1");

    http::HttpResponse resp = service.buildResponse(req);
    TEST_ASSERT(resp.statusCode == 404);
    return true;
}

bool test_service_post_echo()
{
    http::HttpService service;
    http::HttpRequest req = makeRequest("POST", "/echo", "HTTP/1.1");
    req.body = "abc";

    http::HttpResponse resp = service.buildResponse(req);
    TEST_ASSERT(resp.statusCode == 200);
    TEST_ASSERT(resp.body == "abc");
    TEST_ASSERT(resp.headers["X-Body-Size"] == "3");
    return true;
}

bool test_service_delete_resource()
{
    http::HttpService service;
    http::HttpRequest req = makeRequest("DELETE", "/resource", "HTTP/1.1");

    http::HttpResponse resp = service.buildResponse(req);
    TEST_ASSERT(resp.statusCode == 204);
    TEST_ASSERT(resp.body.empty());
    return true;
}

bool test_service_version_not_supported()
{
    http::HttpService service;
    http::HttpRequest req = makeRequest("GET", "/", "HTTP/1.0");

    http::HttpResponse resp = service.buildResponse(req);
    TEST_ASSERT(resp.statusCode == 505);
    TEST_ASSERT(!resp.keepAlive);
    return true;
}
