#include "Server.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

static bool isWrongNameChar(const char c)
{
    if (c == ' ' || c == '\a' || c == '\007' || c == ':' || c == ',' || c == '#')
        return true;
    return false;
}

void Server::cmdNick(const std::string &args, int fd) 
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = sender->getNick().empty() ? "*" : sender->getNick();
    std::istringstream iss(args);
    std::string nickName;
    if (!(iss >> nickName))
    {
        ErrorReply::sendNoNickNameGiven(fd, nick);
        return;
    }
    if (nickName.empty() || nickName.size() > MAX_NICKNAME_SIZE)
    {
        ErrorReply::sendNoNickNameGiven(fd, nick);
        return;
    }
    for (unsigned int idx = 0; idx < nickName.size(); idx++)
    {
        if (isWrongNameChar(nickName[idx]))
        {
            ErrorReply::sendErroneusNickname(fd, nick, nickName);
            return;           
        }
    }
    Client *client = getClientByNick(nickName);
    if (client && client != sender)
    {
        ErrorReply::sendNickNameInUse(fd, nick, nickName);
        return;
    }
    std::string oldNick = sender->getNick();
    sender->setNick(nickName);
    SuccessReply::sendNickChanged(fd, oldNick, nickName);
    if (sender->isRegistered() && !sender->isWelcomed())
    {
        sender->setWelcomed(true);
        std::string welcomeMsg = ":irc 001 " + nickName + " :Welcome to the Internet Relay Network\r\n";
        send(fd, welcomeMsg.c_str(), welcomeMsg.size(), 0);
    }
}