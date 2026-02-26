#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

class Client
{
private:
    int         _clientFd;
    std::string _clientIp;

public:
    Client();
    int         getFd() const;
    std::string getIp() const;

    void setFd(int fd);
    void setIp(const std::string& ip);
};

#endif