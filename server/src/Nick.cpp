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
    if (nickName.empty() || nickName.length() > MAX_NICKNAME_SIZE)
    {
        ErrorReply::sendNeedMoreParams(fd, user, "NICK");
        return;
    }
    if ()
    Client *client = getClientByNick(nickName);
    if (client.getNick() == nickName)
    {
        return;
    }
    
    sender->setNick(nickName);
}

bool isValidNickNameCharacter(char c)
{
    
}