#include "Server.hpp"
#include "ErrorReplies.hpp"

static bool isChannelTarget(std::string target)
{
    while (!target.empty() && (target[0] == '@' || target[0] == '+' || target[0] == '%' || target[0] == '&' || target[0] == '~'))
        target.erase(0, 1);
    return !target.empty() && target[0] == '#';
}

static std::string normalizeChannelTarget(std::string target)
{
    while (!target.empty() && (target[0] == '@' || target[0] == '+' || target[0] == '%' || target[0] == '&' || target[0] == '~'))
        target.erase(0, 1);
    return target;
}

void Server::cmdPrivMsg(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = sender->getNick().empty() ? "*" : sender->getNick();
    std::istringstream iss(args);
    std::string targets, messageText;
    if (!(iss >> targets))
    {
        ErrorReply::sendNoRecipient(fd, nick);
        return;
    }
    std::getline(iss, messageText);
    if (!messageText.empty() && messageText[0] == ' ')
        messageText.erase(0, 1);
    if (!messageText.empty() && messageText[0] == ':')
        messageText.erase(0, 1);
    if (messageText.empty())
    {
        ErrorReply::sendNoTextToSend(fd, nick);
        return;
    }
    std::stringstream ss(targets);
    std::string target;
    while (std::getline(ss, target, ','))
    {
        if (target.empty())
            continue;
        if (isChannelTarget(target))
        {
            std::string chanName = normalizeChannelName(normalizeChannelTarget(target));
            Channel *channel = getChannel(chanName);
            if (!channel)
            {
                ErrorReply::sendNoSuchChannel(fd, nick, chanName);
                continue;
            }
            if (!channel->isClient(*sender))
            {
                ErrorReply::sendCannotSendToChan(fd, nick, chanName);
                continue;
            }
            channel->broadcastMsg(*sender, " PRIVMSG " + chanName + " :" + messageText, _clients);
        }
        else
        {
            Client *dest = getClientByNick(target);
            if (!dest)
            {
                ErrorReply::sendNoSuchNick(fd, nick, target);
                continue;
            }
            std::string full = ":" + sender->getNick() + "!" + sender->getUser() + "@" + sender->getHost()
                             + " PRIVMSG " + dest->getNick() + " :" + messageText + "\r\n";
            dest->forward(full);
        }
    }
}
