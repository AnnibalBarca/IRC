#include "Channel.hpp"
#include <algorithm>

Channel::Channel() : name(""), topic(""), passwd(""), limit(0), createdTime(std::time(NULL)) {}

Channel::Channel(std::string name, Client &owner)
    : name(name), topic(""), passwd(""), limit(0), createdTime(std::time(NULL))
{
    this->clientFds.push_back(owner.getFd());
    this->opFds.push_back(owner.getFd());
    this->modes.push_back('t');
}
Channel::~Channel() {}

std::string Channel::getName()
{
    return this->name;
}

std::string Channel::getTopic()
{
    return this->topic;
}
void Channel::setTopic(std::string topic) { this->topic = topic; }
std::string Channel::getPasswd() { return this->passwd; }
void Channel::setPasswd(std::string passwd) { this->passwd = passwd; }
size_t Channel::getLimit() { return this->limit; }
void Channel::setLimit(size_t limit) { this->limit = limit; }
std::vector<int> &Channel::getClientFds() { return this->clientFds; }
std::vector<int> &Channel::getOpFds() { return this->opFds; }
bool Channel::isEmpty() { return this->clientFds.empty(); }

bool Channel::isClient(Client &c)
{
    for (size_t i = 0; i < clientFds.size(); i++)
        if (clientFds[i] == c.getFd())
            return true;
    return false;
}

void Channel::addClient(Client &c)
{
    if (limit > 0 && clientFds.size() >= limit)
    {
        c.sendReply("471", c.getNick() + " " + this->name + " :Cannot join channel (+l)");
        return;
    }
    if (!isClient(c))
        clientFds.push_back(c.getFd());
}

void Channel::removeClient(Client &c)
{
    for (std::vector<int>::iterator it = clientFds.begin(); it != clientFds.end(); ++it)
        if (*it == c.getFd())
        {
            clientFds.erase(it);
            break;
        }
}

bool Channel::isOp(Client &op)
{
    for (size_t i = 0; i < opFds.size(); i++)
        if (opFds[i] == op.getFd())
            return true;
    return false;
}

void Channel::addOp(Client &op)
{
    if (!isOp(op))
        opFds.push_back(op.getFd());
}

void Channel::removeOp(Client &op)
{
    for (std::vector<int>::iterator it = opFds.begin(); it != opFds.end(); ++it)
        if (*it == op.getFd())
        {
            opFds.erase(it);
            break;
        }
}

bool Channel::isInvited(Client &c)
{
    for (size_t i = 0; i < invitedFds.size(); i++)
        if (invitedFds[i] == c.getFd())
            return true;
    return false;
}

void Channel::addInvite(Client &c)
{
    if (!isInvited(c))
        invitedFds.push_back(c.getFd());
}

void Channel::removeInvite(Client &c)
{
    for (std::vector<int>::iterator it = invitedFds.begin(); it != invitedFds.end(); ++it)
        if (*it == c.getFd())
        {
            invitedFds.erase(it);
            break;
        }
}

// Broadcast à tous sauf l'émetteur
void Channel::broadcastMsg(Client &sender, const std::string &msg, std::vector<Client> &allClients)
{
    std::string full = ":" + sender.getNick() + "!" + sender.getUser() + "@" + sender.getHost() + msg + "\r\n";
    for (size_t i = 0; i < clientFds.size(); i++)
    {
        if (clientFds[i] == sender.getFd())
            continue;
        for (size_t j = 0; j < allClients.size(); j++)
            if (allClients[j].getFd() == clientFds[i])
                allClients[j].forward(full);
    }
}

// Broadcast à tout le monde y compris l'émetteur
void Channel::broadcast(Client &sender, const std::string &msg, std::vector<Client> &allClients)
{
    std::string full = ":" + sender.getNick() + "!" + sender.getUser() + "@" + sender.getHost() + msg + "\r\n";
    for (size_t i = 0; i < clientFds.size(); i++)
        for (size_t j = 0; j < allClients.size(); j++)
            if (allClients[j].getFd() == clientFds[i])
                allClients[j].forward(full);
}

bool Channel::isMode(char mode)
{
    for (size_t i = 0; i < modes.size(); i++)
        if (modes[i] == mode)
            return true;
    return false;
}

void Channel::addMode(char mode)
{
    if (!isMode(mode))
        modes.push_back(mode);
}

void Channel::removeMode(char mode)
{
    for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
        if (*it == mode)
        {
            modes.erase(it);
            break;
        }
}

std::string Channel::getModes()
{
    std::string result = "+";
    for (size_t i = 0; i < modes.size(); i++)
        result += modes[i];
    return result;
}

std::time_t Channel::getCreationTime() const
{
    return this->createdTime;
}
