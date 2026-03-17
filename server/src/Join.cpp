#include "Server.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

static bool isWrongNameChar(const char nameChar)
{
    if (nameChar == ' ' || nameChar == '\a' || nameChar == '\007' || nameChar == ':' || nameChar == ',')
        return false;
    return true;
}

static bool isValidChanName(std::string chanName)
{
    if (chanName.empty() || chanName.size() < 2 || chanName.size() > MAX_CHANNELNAME_SIZE)
        return false;
    if (chanName[0] != '#')
        return false;
    for (size_t idx = 1; idx < chanName.length(); idx++)
    {
        if (!isWrongNameChar(chanName[idx]))
            return false;
    }
    return true;
}

void Server::cmdJoin(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string user = sender->getUser().empty() ? "*" : sender->getUser();
    std::istringstream iss(args);
    std::string chanName, keyName;
    if (!(iss >> chanName))
    {
        ErrorReply::sendNeedMoreParams(fd, user, "JOIN");
        return;
    }
    if (!isValidChanName(chanName))
    {
        ErrorReply::sendNoSuchChannel(fd, user, chanName);
        return;
    }
    for (size_t idx = 0; idx < chanName.length(); idx++)
    {
        chanName[idx] = toupper(chanName[idx]);
    }
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        _channels.push_back(Channel(chanName, *sender));
        channel = &_channels.back();
        sender->addChan(channel);
        std::string joinMsg = ":" + sender->getNick() + "!" + sender->getUser() + "@" + sender->getHost() + " JOIN " + channel->getName() + "\r\n";
        sender->forward(joinMsg);
        channel->broadcast(*sender, " JOIN " + channel->getName(), _clients);
        std::string topicMsg = "331 " + user + " " + chanName + " :No topic is set\r\n";
        send(fd, topicMsg.c_str(), topicMsg.size(), 0);
        std::string namesMsg = "353 " + user + " = " + chanName + " :" + sender->getNick() + "\r\n";
        send(fd, namesMsg.c_str(), namesMsg.size(), 0);
        std::string endNamesMsg = "366 " + user + " " + chanName + " :End of NAMES list\r\n";
        send(fd, endNamesMsg.c_str(), endNamesMsg.size(), 0);
        SuccessReply::sendJoinConfirmed(fd, user, chanName);
        return;
    }
    if (channel->isClient(*sender))
        return;
    if (iss >> keyName)
    {
        if (!channel->getPasswd().empty())
        {
            if (channel->getPasswd() != keyName)
            {
                ErrorReply::sendBadChannelKey(fd, user, chanName);
                return;
            }
        }
    }
    else
    {
        if (!channel->getPasswd().empty())
        {
            ErrorReply::sendBadChannelKey(fd, user, chanName);
            return;
        }
    }
    if (channel->getLimit() != 0 && channel->getClientFds().size() >= channel->getLimit())
    {
        ErrorReply::sendChannelIsFull(fd, user, chanName);
        return;
    }
    if (channel->isMode('i') && !channel->isInvited(*sender))
    {
        ErrorReply::sendInviteOnlyChan(fd, user, chanName);
        return;
    }
    channel->addClient(*sender);
    sender->addChan(channel);
    std::string joinMsg = ":" + sender->getNick() + "!" + sender->getUser() + "@" + sender->getHost() + " JOIN " + channel->getName() + "\r\n";
    sender->forward(joinMsg);
    channel->broadcast(*sender, " JOIN " + channel->getName(), _clients);
    if (channel->getTopic().empty())
    {
        std::string topicMsg = "331 " + user + " " + chanName + " :No topic is set\r\n";
        send(fd, topicMsg.c_str(), topicMsg.size(), 0);
    }
    else
    {
        std::string topicMsg = "332 " + user + " " + chanName + " :" + channel->getTopic() + "\r\n";
        send(fd, topicMsg.c_str(), topicMsg.size(), 0);
    }
    std::vector<int> &clientFds = channel->getClientFds();
    std::string namesMsg = "353 " + user + " = " + chanName + " :";
    for (size_t i = 0; i < clientFds.size(); i++)
    {
        Client *c = getClient(clientFds[i]);
        if (c)
        {
            if (channel->isOp(*c))
                namesMsg += "@";
            namesMsg += c->getNick();
            if (i < clientFds.size() - 1)
                namesMsg += " ";
        }
    }
    namesMsg += "\r\n";
    send(fd, namesMsg.c_str(), namesMsg.size(), 0);
    std::string endNamesMsg = "366 " + user + " " + chanName + " :End of NAMES list\r\n";
    send(fd, endNamesMsg.c_str(), endNamesMsg.size(), 0);
    SuccessReply::sendJoinConfirmed(fd, user, chanName);
}
