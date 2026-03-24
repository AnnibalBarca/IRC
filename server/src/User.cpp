#include "Server.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

void Server::cmdUser(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = sender->getNick().empty() ? "*" : sender->getNick();
    std::istringstream iss(args);
    std::string userName, mode, unused;
    if (!(iss >> userName >> mode >> unused))
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "USER");
        return;
    }
    if (sender->isNamed())
    {
        ErrorReply::sendAlreadyRegistered(fd, nick);
        return;
    }
    if (mode != "0" && mode != "*")
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "USER");
        return;
    }
    if (unused != "*")
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "USER");
        return;
    }
    std::string trailing;
    std::getline(iss, trailing);
    if (!trailing.empty() && trailing[0] == ' ')
        trailing.erase(0, 1);
    if (trailing.empty() || trailing[0] != ':')
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "USER");
        return;
    }
    sender->setUser(userName);
    SuccessReply::sendUserSet(fd, sender->getNick().empty() ? "*" : sender->getNick());
    if (sender->isRegistered() && !sender->isWelcomed())
    {
        sender->setWelcomed(true);
        std::string welcomeMsg = ":irc 001 " + sender->getNick() + " :Welcome to the Internet Relay Network\r\n";
        queueToFd(fd, welcomeMsg);
    }
}
