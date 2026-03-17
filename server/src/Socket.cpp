#include "Socket.hpp"

Socket::Socket() : _fd(-1)
{
    std::memset(&_addr, 0, sizeof(_addr));
}

Socket::~Socket()
{
    closeSocket();
}

void    Socket::create()
{
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
        throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));
}

void    Socket::setReuseAddr()
{
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error(std::string("setsockopt() failed: ") + strerror(errno));
}

void    Socket::bindTo(int port)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family      = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port        = htons(port);
    if (bind(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) < 0)
        throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
}

void    Socket::startListening()
{
    if (listen(_fd, SOMAXCONN) < 0)
        throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
}

void    Socket::setNonBlocking()
{
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error(std::string("fcntl() failed: ") + strerror(errno));
}

int     Socket::getFd() const
{
    return _fd;
}

const struct sockaddr_in&   Socket::getAddr() const
{
    return _addr;
}

void    Socket::closeSocket()
{
    if (_fd >= 0)
    {
        close(_fd);
        _fd = -1;
    }
}
