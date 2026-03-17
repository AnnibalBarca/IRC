#include "Server.hpp"
#include "ErrorReplies.hpp"
void Server::cmdUser(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string user = sender->getUser().empty() ? "*" : sender->getUser();
    std::istringstream iss(args);
    std::string userName, mode, unused;
    if (!(iss >> userName >> mode >> unused))
    {
        ErrorReply::sendNeedMoreParams(fd, user, "USER");
        return;
    }
    if (sender->isNamed())
    {
        ErrorReply::sendAlreadyRegistered(fd, user);
        return;
    }
    if (mode != "0" && mode != "*")
    {
        ErrorReply::sendNeedMoreParams(fd, user, "USER");
        return;
    }
    if (unused != "*")
    {
        ErrorReply::sendNeedMoreParams(fd, user, "USER");
        return;
    }
    std::string trailing;
    std::getline(iss, trailing);
    if (!trailing.empty() && trailing[0] == ' ')
        trailing.erase(0, 1);
    if (trailing.empty() || trailing[0] != ':')
    {
        ErrorReply::sendNeedMoreParams(fd, user, "USER");
        return;
    }
    sender->setUser(userName);
}
