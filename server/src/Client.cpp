#include "Client.hpp"
#include "Server.hpp"
#include <algorithm>

Client::Client() : nick(""), user(""), ip(""), host(""), chans(), buf(""), outBuf(""), fd(-1), auth(false), welcomed(false) {}

Client::Client(int fd, std::string ip, std::string host)
    : nick(""), user(""), ip(ip), host(host), chans(), buf(""), outBuf(""), fd(fd), auth(false), welcomed(false) {}

Client::~Client() {}

void        Client::setFd(int fd)               { this->fd = fd; }

int         Client::getFd() const               { return this->fd; }

void        Client::setAuth(bool auth)          { this->auth = auth; }

bool        Client::getAuth()                   { return this->auth; }

bool        Client::isAuth()                    { return this->auth; }

void        Client::setAuthentified()           { this->auth = true; }

bool        Client::isWelcomed()                { return this->welcomed; }

void        Client::setWelcomed(bool val)       { this->welcomed = val; }

std::string Client::getNick()                   { return this->nick; }

void        Client::setNick(std::string nick)   { this->nick = nick; }

std::string Client::getUser()                   { return this->user; }

void        Client::setUser(std::string user)   { this->user = user; }

std::string Client::getHost()                   { return this->host; }

void        Client::setHost(std::string host)   { this->host = host; }

std::string Client::getIp()                     { return this->ip; }

void        Client::setIp(std::string ip)       { this->ip = ip; }

bool        Client::isRegistered()              { return (!this->nick.empty() && !this->user.empty()); }

bool        Client::isNamed()                   { return (!this->user.empty()); }

void        Client::setBuf(std::string buf)     { this->buf = buf; }

void        Client::addBuf(std::string buf)     { this->buf += buf; }

std::string &Client::getBuf()                   { return this->buf; }

void        Client::addOutBuf(const std::string &msg) { this->outBuf += msg; }

std::string &Client::getOutBuf()                { return this->outBuf; }

std::vector<std::string> &Client::getChans()    { return this->chans; }

bool Client::operator==(const Client &c) const  { return this->getFd() == c.getFd(); }

void Client::forward(std::string msg)
{
    this->addOutBuf(msg);
    Server *server = Server::instance();
    if (server)
        server->enablePollOut(this->fd);
}

void Client::sendMsg(std::string msg, Client &c) { c.forward(msg); }

void Client::sendReply(std::string code, std::string msg)
{
    this->forward(":" + this->host + " " + code + " " + msg + "\r\n");
}

void Client::addChan(const std::string &chanName)
{
    if (std::find(this->chans.begin(), this->chans.end(), chanName) == this->chans.end())
        this->chans.push_back(chanName);
}

void Client::removeChan(const std::string &chanName)
{
    for (std::vector<std::string>::iterator it = this->chans.begin(); it != this->chans.end(); ++it)
        if (*it == chanName) { this->chans.erase(it); break; }
}
