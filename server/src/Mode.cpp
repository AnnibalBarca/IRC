#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"
#include <cstdlib>

void Server::cmdMode(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = sender->getNick().empty() ? "*" : sender->getNick();
    std::istringstream iss(args);
    std::string chanName, modeStr;
    if (!(iss >> chanName))
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "MODE");
        return;
    }
    chanName = normalizeChannelName(chanName);
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        ErrorReply::sendNoSuchChannel(fd, nick, chanName);
        return;
    }
    if (!(iss >> modeStr))
    {
        std::string rpl324 = "324 " + nick + " " + chanName + " " + channel->getModes() + "\r\n";
        send(fd, rpl324.c_str(), rpl324.size(), 0);
        std::stringstream ss;
        ss << channel->getCreationTime();
        std::string rpl329 = "329 " + nick + " " + chanName + " " + ss.str() + "\r\n";
        send(fd, rpl329.c_str(), rpl329.size(), 0);
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
    std::vector<std::string> params;
    std::string p;
    while (iss >> p)
        params.push_back(p);
    size_t paramIdx = 0;
    bool adding = true;
    std::string appliedPlus, appliedMinus;
    std::string appliedPlusParams, appliedMinusParams;
    for (size_t i = 0; i < modeStr.size(); ++i)
    {
        char c = modeStr[i];
        if (c == '+')
        {
            adding = true;
            continue;
        }
        if (c == '-')
        {
            adding = false;
            continue;
        }
        bool success = false;
        switch (c)
        {
            case 'i':
                success = modeI(channel, adding);
                break;
            case 't':
                success = modeT(channel, adding);
                break;
            case 'k':
                success = modeK(channel, nick, adding, paramIdx, params, fd);
                break;
            case 'o':
                success = modeO(channel, nick, chanName, adding, paramIdx, params, fd);
                break;
            case 'l':
                success = modeL(channel, nick, adding, paramIdx, params, fd);
                break;
            default:
                if (std::isalpha((unsigned char)c))
                    ErrorReply::sendUnknownModeChar(fd, nick, c);
                continue;
        }
        if (success)
        {
            if (adding)
                appliedPlus += c;
            else
                appliedMinus += c;
            if (c == 'k' && adding && paramIdx > 0)
                appliedPlusParams += " " + params[paramIdx - 1];
            else if (c == 'o' && paramIdx > 0)
                (adding ? appliedPlusParams : appliedMinusParams) += " " + params[paramIdx - 1];
            else if (c == 'l' && adding && paramIdx > 0)
                appliedPlusParams += " " + params[paramIdx - 1];
        }
    }
    std::string finalModes, finalParams;
    if (!appliedPlus.empty())
    {
        finalModes += "+" + appliedPlus;
        finalParams += appliedPlusParams;
    }
    if (!appliedMinus.empty())
    {
        finalModes += "-" + appliedMinus;
        finalParams += appliedMinusParams;
    }
    if (!finalModes.empty())
    {
        std::string msg = " MODE " + chanName + " " + finalModes + finalParams;
        channel->broadcast(*sender, msg, _clients);
        SuccessReply::sendModeUpdated(fd, nick, chanName, finalModes + finalParams);
    }
}

bool Server::modeI(Channel *channel, bool adding)
{
    if (adding)
    {
        if (channel->isMode('i'))
            return false;
        modeHandling(channel, 'i', true);
        return true;
    }
    if (!channel->isMode('i'))
        return false;
    modeHandling(channel, 'i', false);
    return true;
}

bool Server::modeT(Channel *channel, bool adding)
{
    if (adding)
    {
        if (channel->isMode('t'))
            return false;
        modeHandling(channel, 't', true);
        return true;
    }
    if (!channel->isMode('t'))
        return false;
    modeHandling(channel, 't', false);
    return true;
}

bool Server::modeK(Channel *channel, const std::string &user, bool adding,
                   size_t &paramIdx, const std::vector<std::string> &params, int fd)
{
    if (adding)
    {
        if (paramIdx >= params.size())
        {
            ErrorReply::sendNeedMoreParams(fd, user, "MODE");
            return false;
        }
        std::string key = params[paramIdx++];
        channel->setPasswd(key);
        modeHandling(channel, 'k', true);
        return true;
    }
    if (!channel->isMode('k'))
        return false;
    channel->setPasswd("");
    modeHandling(channel, 'k', false);
    return true;
}

bool Server::modeO(Channel *channel, const std::string &user, const std::string &chanName, bool adding,
                   size_t &paramIdx, const std::vector<std::string> &params, int fd)
{
    if (paramIdx >= params.size())
    {
        ErrorReply::sendNeedMoreParams(fd, user, "MODE");
        return false;
    }
    std::string targetNick = params[paramIdx++];
    Client *target = getClientByNick(targetNick);
    if (!target)
    {
        ErrorReply::sendNoSuchNick(fd, user, targetNick);
        return false;
    }
    if (!channel->isClient(*target))
    {
        ErrorReply::sendUserNotInChannel(fd, user, targetNick, chanName);
        return false;
    }
    if (adding)
    {
        if (channel->isOp(*target))
            return false;
        channel->addOp(*target);
        return true;
    }
    if (!channel->isOp(*target))
        return false;
    channel->removeOp(*target);
    return true;
}

bool Server::modeL(Channel *channel, const std::string &user, bool adding,
                   size_t &paramIdx, const std::vector<std::string> &params, int fd)
{
    if (adding)
    {
        if (paramIdx >= params.size())
        {
            ErrorReply::sendNeedMoreParams(fd, user, "MODE");
            return false;
        }
        const std::string &limitParam = params[paramIdx++];
        char *end = NULL;
        unsigned long parsed = std::strtoul(limitParam.c_str(), &end, 10);
        if (*end != '\0' || parsed == 0)
            return false;
        channel->setLimit((size_t)parsed);
        modeHandling(channel, 'l', true);
        return true;
    }
    if (!channel->isMode('l'))
        return false;
    channel->setLimit(0);
    modeHandling(channel, 'l', false);
    return true;
}

void Server::modeHandling(Channel *channel, char mod, bool addOrRemove)
{
    if (!(mod == 'i' || mod == 'k' || mod == 'l' || mod == 'o' || mod == 't'))
        return;
    if (addOrRemove == true)
        channel->addMode(mod);
    else
        channel->removeMode(mod);
}
