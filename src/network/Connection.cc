#include "network/Connection.hh"

#include <cerrno>
#include <unistd.h>

namespace net
{
    Connection::Connection(SocketFD fd, std::size_t maxReadBufferBytes)
        : _fd(fd),
          _closed(false),
          _maxReadBufferBytes(maxReadBufferBytes)
    {
    }

    Connection::~Connection()
    {
        if (!_closed)
            close(_fd);
    }

    SocketFD Connection::getFd() const
    {
        return _fd;
    }

    bool Connection::wantsRead() const
    {
        return !_closed;
    }

    bool Connection::wantsWrite() const
    {
        return !_closed && !_writeBuffer.empty();
    }

    bool Connection::isClosed() const
    {
        return _closed;
    }

    void Connection::onReadable()
    {
        if (_closed)
            return;

        char buffer[4096];

        while (true)
        {
            ssize_t bytes = read(_fd, buffer, sizeof(buffer));
            if (bytes > 0)
            {
                if (_readBuffer.size() + static_cast<std::size_t>(bytes) > _maxReadBufferBytes)
                {
                    onDisconnect();
                    return;
                }

                _readBuffer.insert(_readBuffer.end(),
                                   buffer,
                                   buffer + static_cast<std::size_t>(bytes));
                return;
            }

            if (bytes == 0)
            {
                onDisconnect();
                return;
            }

            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;

            onDisconnect();
            return;
        }
    }

    void Connection::onWritable()
    {
        if (_closed)
            return;

        while (true)
        {
            if (_writeBuffer.empty())
                return;

            ssize_t bytes = write(_fd, &_writeBuffer[0], _writeBuffer.size());

            if (bytes > 0)
            {
                _writeBuffer.erase(_writeBuffer.begin(),
                                   _writeBuffer.begin() + static_cast<std::size_t>(bytes));
                return;
            }

            if (bytes == 0)
                return;

            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;

            onDisconnect();
            return;
        }
    }

    void Connection::onDisconnect()
    {
        if (_closed)
            return;

        _closed = true;
        close(_fd);
    }

    const std::vector<char>& Connection::getReadBuffer() const
    {
        return _readBuffer;
    }

    void Connection::clearReadBuffer()
    {
        _readBuffer.clear();
    }

    void Connection::appendToWriteBuffer(const char* data, std::size_t size)
    {
        if (_closed || data == NULL || size == 0)
            return;

        _writeBuffer.insert(_writeBuffer.end(), data, data + size);
    }

    std::size_t Connection::pendingWriteBytes() const
    {
        return _writeBuffer.size();
    }
}
