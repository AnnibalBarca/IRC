#include "Client.hpp"

Client::Client() : _clientFd(-1)
{}

int Client::getFd() const
{
    return _clientFd;
}

std::string Client::getIp() const
{
    return _clientIp;
}

void Client::setFd(int fd)
{
    _clientFd = fd;
}

void Client::setIp(const std::string& ip)
{
    _clientIp = ip;
}