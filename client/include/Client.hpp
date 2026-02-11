#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client
{
private:
    int _clientFd;
    std::string _clientIp;
public:
    int getFd();

    void setFd();
    void setIp(std::string clientIp);
    
}

#endif