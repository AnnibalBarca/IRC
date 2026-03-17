#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"

void Server::cmdInvite(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = sender->getNick().empty() ? "*" : sender->getNick();
    std::istringstream iss(args);
    std::string targetNick, chanName;
    if (!(iss >> targetNick >> chanName))
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "INVITE");
        return;
    }
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
    if (channel->isMode('i') && !channel->isOp(*sender))
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
    if (channel->isClient(*target))
    {
        ErrorReply::sendUserOnChannel(fd, nick, targetNick, chanName);
        return;
    }
    channel->addInvite(*target);
    std::string rpl = "341 " + nick + " " + targetNick + " " + chanName + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
    std::string inviteMsg = ":" + nick + " INVITE " + targetNick + " " + chanName + "\r\n";
    send(target->getFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
}
