#include "http/HttpParser.hh"

#include <cstdlib>

namespace http
{
    static std::string trim(const std::string& value)
    {
        std::size_t begin = 0;
        while (begin < value.size() && (value[begin] == ' ' || value[begin] == '\t'))
            ++begin;

        std::size_t end = value.size();
        while (end > begin && (value[end - 1] == ' ' || value[end - 1] == '\t'))
            --end;

        return value.substr(begin, end - begin);
    }

    static bool parseRequestLine(const std::string& line,
                                 std::string& method,
                                 std::string& target,
                                 std::string& version)
    {
        std::size_t firstSpace = line.find(' ');
        if (firstSpace == std::string::npos)
            return false;

        std::size_t secondSpace = line.find(' ', firstSpace + 1);
        if (secondSpace == std::string::npos)
            return false;

        if (line.find(' ', secondSpace + 1) != std::string::npos)
            return false;

        method = line.substr(0, firstSpace);
        target = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        version = line.substr(secondSpace + 1);

        if (method.empty() || target.empty() || version.empty())
            return false;

        if (version.find("HTTP/") != 0)
            return false;

        return true;
    }

    static bool parseContentLength(const std::map<std::string, std::string>& headers,
                                   std::size_t& contentLength)
    {
        contentLength = 0;

        std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
        if (it == headers.end())
            return true;

        const std::string& value = it->second;
        if (value.empty())
            return false;

        char* endPtr = NULL;
        long parsed = std::strtol(value.c_str(), &endPtr, 10);
        if (endPtr == value.c_str() || *endPtr != '\0' || parsed < 0)
            return false;

        contentLength = static_cast<std::size_t>(parsed);
        return true;
    }

    HttpParser::ParseResult HttpParser::parse(const std::string& input,
                                              HttpRequest& request,
                                              std::size_t& consumedBytes) const
    {
        consumedBytes = 0;

        const std::size_t headerEnd = input.find("\r\n\r\n");
        if (headerEnd == std::string::npos)
            return PARSE_INCOMPLETE;

        const std::string headerBlock = input.substr(0, headerEnd);

        std::size_t lineStart = 0;
        std::size_t lineEnd = headerBlock.find("\r\n");
        const std::string requestLine = (lineEnd == std::string::npos)
                                            ? headerBlock
                                            : headerBlock.substr(0, lineEnd);

        HttpRequest parsedRequest;
        if (!parseRequestLine(requestLine,
                              parsedRequest.method,
                              parsedRequest.target,
                              parsedRequest.version))
        {
            return PARSE_BAD_REQUEST;
        }

        while (lineEnd != std::string::npos)
        {
            lineStart = lineEnd + 2;
            lineEnd = headerBlock.find("\r\n", lineStart);

            std::string line;
            if (lineEnd == std::string::npos)
                line = headerBlock.substr(lineStart);
            else
                line = headerBlock.substr(lineStart, lineEnd - lineStart);

            if (line.empty())
                break;

            const std::size_t sep = line.find(':');
            if (sep == std::string::npos)
                return PARSE_BAD_REQUEST;

            const std::string key = trim(line.substr(0, sep));
            const std::string value = trim(line.substr(sep + 1));
            if (key.empty())
                return PARSE_BAD_REQUEST;

            parsedRequest.headers[key] = value;
        }

        std::size_t contentLength = 0;
        if (!parseContentLength(parsedRequest.headers, contentLength))
            return PARSE_BAD_REQUEST;

        const std::size_t totalSize = headerEnd + 4 + contentLength;
        if (input.size() < totalSize)
            return PARSE_INCOMPLETE;

        parsedRequest.body = input.substr(headerEnd + 4, contentLength);

        std::map<std::string, std::string>::const_iterator connectionHeader =
            parsedRequest.headers.find("Connection");

        if (parsedRequest.version == "HTTP/1.0")
        {
            parsedRequest.keepAlive = (connectionHeader != parsedRequest.headers.end() &&
                                       connectionHeader->second == "keep-alive");
        }
        else
        {
            parsedRequest.keepAlive = !(connectionHeader != parsedRequest.headers.end() &&
                                        connectionHeader->second == "close");
        }

        consumedBytes = totalSize;
        request = parsedRequest;
        return PARSE_OK;
    }
}
