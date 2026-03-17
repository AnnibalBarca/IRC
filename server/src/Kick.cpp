#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"

void Server::cmdKick(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = sender->getNick().empty() ? "*" : sender->getNick();
    std::istringstream iss(args);
    std::string chanName, targetNick, reason;
    if (!(iss >> chanName >> targetNick))
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "KICK");
        return;
    }
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);
    if (reason.empty())
        reason = nick;
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        ErrorReply::sendNoSuchChannel(fd, nick, chanName);
        return;
    }
    if (!channel->isClient(*sender))
    {
        ErrorReply::sendNotOnChannel(fd, nick, chanName);
        return;
    }
    if (!channel->isOp(*sender))
    {
        ErrorReply::sendChanOpPrivsNeeded(fd, nick, chanName);
        return;
    }
    Client *target = getClientByNick(targetNick);
    if (!target)
    {
        ErrorReply::sendNoSuchNick(fd, nick, targetNick);
        return;
    }
    if (!channel->isClient(*target))
    {
        ErrorReply::sendUserNotInChannel(fd, nick, targetNick, chanName);
        return;
    }
    std::string kickMsg = " KICK " + chanName + " " + targetNick + " :" + reason;
    channel->broadcast(*sender, kickMsg, _clients);
    channel->removeClient(*target);
    target->removeChan(channel);
    if (channel->isEmpty())
    {
        for (size_t i = 0; i < _channels.size(); i++)
        {
            if (&_channels[i] == channel)
            {
                _channels.erase(_channels.begin() + i);
                break;
            }
        }
    }
}
