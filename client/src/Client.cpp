#include "Client.hpp"
#include "Channel.hpp"
#include <algorithm>

std::runtime_error Client::disconnected = std::runtime_error("Client disconnected");

Client::Client(int fd, std::string ip, std::string host)
    : nick(""), user(""), ip(ip), host(host), chans(), buf(""), fd(fd), auth(false), welcomed(false) {}
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
bool        Client::isNamed()                   { return (!this->nick.empty() && !this->user.empty()); }
void        Client::setBuf(std::string buf)     { this->buf = buf; }
void        Client::addBuf(std::string buf)     { this->buf += buf; }
std::string &Client::getBuf()                   { return this->buf; }

std::vector<Channel *> &Client::getChans()      { return this->chans; }

bool Client::operator==(const Client &c) const  { return this->getFd() == c.getFd(); }

void Client::forward(std::string msg)
{
    send(this->fd, msg.c_str(), msg.size(), MSG_DONTWAIT);
}

void Client::sendMsg(std::string msg, Client &c) { c.forward(msg); }

void Client::sendReply(std::string code, std::string msg)
{
    this->forward(":" + this->host + " " + code + " " + msg + "\r\n");
}

void Client::addChan(Channel *chan) { this->chans.push_back(chan); }

void Client::removeChan(Channel *chan)
{
    for (std::vector<Channel *>::iterator it = this->chans.begin(); it != this->chans.end(); ++it)
        if (*it == chan) { this->chans.erase(it); break; }
}

void Client::disconnect()
{
    std::string quit_msg = ":" + this->nick + "!" + this->user + "@" + this->host + " QUIT :Leaving\r\n";
    std::vector<int> to_notify;
    for (std::vector<Channel *>::iterator it = this->chans.begin(); it != this->chans.end(); ++it)
    {
        std::vector<int> &fds = (*it)->getClientFds();
        for (size_t i = 0; i < fds.size(); i++)
            if (fds[i] != this->fd) to_notify.push_back(fds[i]);
        (*it)->removeClient(*this);
        (*it)->removeOp(*this);
    }
    this->chans.clear();
    std::vector<int> notified;
    for (size_t i = 0; i < to_notify.size(); i++)
    {
        if (std::find(notified.begin(), notified.end(), to_notify[i]) == notified.end())
        {
            send(to_notify[i], quit_msg.c_str(), quit_msg.size(), MSG_DONTWAIT);
            notified.push_back(to_notify[i]);
        }
    }
}