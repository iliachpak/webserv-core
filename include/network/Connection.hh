#ifndef SOCKET_CONNECTION_HH
#define SOCKET_CONNECTION_HH

#include <cstddef>
#include <vector>

#include "network/NetworkTypes.hh"

namespace net
{
    class Connection
    {
    public:
        explicit Connection(SocketFD fd, std::size_t maxReadBufferBytes = 1024 * 1024);
        ~Connection();

        SocketFD getFd() const;

        bool wantsRead() const;
        bool wantsWrite() const;
        bool isClosed() const;

        void onReadable();
        void onWritable();
        void onDisconnect();

        const std::vector<char>& getReadBuffer() const;
        void clearReadBuffer();
        void appendToWriteBuffer(const char* data, std::size_t size);
        std::size_t pendingWriteBytes() const;

    private:
        Connection(const Connection&);
        Connection& operator=(const Connection&);

    private:
        SocketFD     _fd;
        bool         _closed;
        std::size_t  _maxReadBufferBytes;

        std::vector<char> _readBuffer;
        std::vector<char> _writeBuffer;
    };
}

#endif
