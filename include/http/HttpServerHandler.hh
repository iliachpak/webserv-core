#ifndef HTTP_SERVER_HANDLER_HH
#define HTTP_SERVER_HANDLER_HH

#include <map>
#include <set>
#include <string>

#include "http/HttpParser.hh"
#include "http/HttpService.hh"
#include "network/INetworkHandler.hh"

namespace http
{
    class HttpServerHandler : public net::INetworkHandler
    {
    public:
        HttpServerHandler();
        virtual ~HttpServerHandler();

        virtual void onConnectionData(net::Connection& conn);
        virtual void onConnectionWritable(net::Connection& conn);
        virtual void onConnectionClose(net::SocketFD fd);

    private:
        void markCloseAfterWrite(net::SocketFD fd);
        bool shouldCloseAfterWrite(net::SocketFD fd) const;

    private:
        HttpParser _parser;
        HttpService _service;
        std::map<net::SocketFD, std::string> _pendingByFd;
        std::set<net::SocketFD> _closeAfterWrite;
    };
}

#endif
