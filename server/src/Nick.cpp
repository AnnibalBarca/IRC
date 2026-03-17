#include "Server.hpp"
#include "ErrorReplies.hpp"

void Server::cmdNick(const std::string &args, int fd) 
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string user = sender->getUser().empty() ? "*" : sender->getUser();
    std::istringstream iss(args);
    std::string nickName;
    if (!(iss >> nickName))
    {
        ErrorReply::sendNeedMoreParams(fd, user, "NICK");
        return;
    }
    if (nickName.empty() || nickName.size() > MAX_NICKNAME_SIZE)
    {
        ErrorReply::sendNeedMoreParams(fd, user, "NICK");
        return;
    }
    for (int idx = 0; idx < nickName.size(); idx++)
    {
        if (!isWrongNameChar())
    }

    Client *client = getClientByNick(nickName);
    if (client->getNick() == nickName)
    {
        ErrorReply::sendNickNameInUse(fd, user, nickName);
        return;
    }
    
    sender->setNick(nickName);
}