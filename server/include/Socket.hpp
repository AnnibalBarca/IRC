#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>  //-> socket(), setsockopt(), bind(), listen(), accept()
#include <netinet/in.h>  //-> sockaddr_in, INADDR_ANY, htons()
#include <fcntl.h>       //-> fcntl(), O_NONBLOCK
#include <unistd.h>      //-> close()
#include <cstring>       //-> std::memset()
#include <cerrno>        //-> errno
#include <stdexcept>     //-> std::runtime_error
#include <string>        //-> std::string

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

#endif