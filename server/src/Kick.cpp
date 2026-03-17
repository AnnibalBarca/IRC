#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

void Server::cmdKick(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string user = sender->getUser().empty() ? "*" : sender->getUser();
    std::istringstream iss(args);
    std::string chanName, targetNick, reason;
    if (!(iss >> chanName >> targetNick))
    {
        ErrorReply::sendNeedMoreParams(fd, user, "KICK");
        return;
    }
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);
    if (reason.empty())
        reason = user;
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        ErrorReply::sendNoSuchChannel(fd, user, chanName);
        return;
    }
    if (!channel->isClient(*sender))
    {
        ErrorReply::sendNotOnChannel(fd, user, chanName);
        return;
    }
    if (!channel->isOp(*sender))
    {
        ErrorReply::sendChanOpPrivsNeeded(fd, user, chanName);
        return;
    }
    Client *target = getClientByNick(targetNick);
    if (!target)
    {
        ErrorReply::sendNoSuchNick(fd, user, targetNick);
        return;
    }
    if (!channel->isClient(*target))
    {
        ErrorReply::sendUserNotInChannel(fd, user, targetNick, chanName);
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
    SuccessReply::sendKickConfirmed(fd, user, targetNick, chanName);
}
