#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client
{
private:
    int _clientFd;
    std::string _clientIp;
public:
    Client(); // const par def . en cpp pas besoin d'aure chose
    int getFd();

    void setFd();
    void setIp(std::string clientIp);
    
}

#endif