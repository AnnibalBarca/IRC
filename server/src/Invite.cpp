#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

void Server::cmdInvite(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string user = sender->getUser().empty() ? "*" : sender->getUser();
    std::istringstream iss(args);
    std::string targetNick, chanName;
    if (!(iss >> targetNick >> chanName))
    {
        ErrorReply::sendNeedMoreParams(fd, user, "INVITE");
        return;
    }
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
    if (channel->isMode('i') && !channel->isOp(*sender))
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
    if (channel->isClient(*target))
    {
        ErrorReply::sendUserOnChannel(fd, user, targetNick, chanName);
        return;
    }
    channel->addInvite(*target);
    std::string rpl = "341 " + user + " " + targetNick + " " + chanName + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
    std::string inviteMsg = ":" + user + " INVITE " + targetNick + " " + chanName + "\r\n";
    send(target->getFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
    SuccessReply::sendInviteConfirmed(fd, user, targetNick, chanName);
}
