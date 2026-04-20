#include "http/HttpServerHandler.hh"

#include "http/HttpRequest.hh"
#include "http/HttpResponse.hh"
#include "network/Connection.hh"

namespace http
{
    HttpServerHandler::HttpServerHandler()
    {
    }

    HttpServerHandler::~HttpServerHandler()
    {
    }

    void HttpServerHandler::onConnectionData(net::Connection& conn)
    {
        const net::SocketFD fd = conn.getFd();

        const std::vector<char>& chunk = conn.getReadBuffer();
        if (!chunk.empty())
            _pendingByFd[fd].append(chunk.begin(), chunk.end());
        conn.clearReadBuffer();

        std::string& pending = _pendingByFd[fd];

        while (true)
        {
            HttpRequest request;
            std::size_t consumed = 0;
            HttpParser::ParseResult result = _parser.parse(pending, request, consumed);

            if (result == HttpParser::PARSE_INCOMPLETE)
                break;

            HttpResponse response;

            if (result == HttpParser::PARSE_BAD_REQUEST)
            {
                response.statusCode = 400;
                response.reason = "Bad Request";
                response.body = "Malformed HTTP request\n";
                response.keepAlive = false;

                pending.clear();
                markCloseAfterWrite(fd);
            }
            else
            {
                response = _service.buildResponse(request);

                if (consumed > pending.size())
                    pending.clear();
                else
                    pending.erase(0, consumed);

                if (!response.keepAlive)
                    markCloseAfterWrite(fd);
            }

            const std::string bytes = response.serialize();
            conn.appendToWriteBuffer(bytes.c_str(), bytes.size());

            if (result == HttpParser::PARSE_BAD_REQUEST)
                break;
        }
    }

    void HttpServerHandler::onConnectionWritable(net::Connection& conn)
    {
        const net::SocketFD fd = conn.getFd();

        if (!shouldCloseAfterWrite(fd))
            return;

        if (conn.pendingWriteBytes() == 0)
            conn.onDisconnect();
    }

    void HttpServerHandler::onConnectionClose(net::SocketFD fd)
    {
        _pendingByFd.erase(fd);
        _closeAfterWrite.erase(fd);
    }

    void HttpServerHandler::markCloseAfterWrite(net::SocketFD fd)
    {
        _closeAfterWrite.insert(fd);
    }

    bool HttpServerHandler::shouldCloseAfterWrite(net::SocketFD fd) const
    {
        return _closeAfterWrite.find(fd) != _closeAfterWrite.end();
    }
}
