#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <string>
class Socket
{
private:
    int                 _fd;
    struct sockaddr_in  _addr;
public:
    Socket();
    ~Socket();
    void    create();
    void    setReuseAddr();
    void    bindTo(int port);
    void    startListening();
    void    setNonBlocking();
    int     getFd()   const;
    const struct sockaddr_in& getAddr() const;
    void    closeSocket();
};
