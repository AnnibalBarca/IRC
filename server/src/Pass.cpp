#include "Server.hpp"
#include "ErrorReplies.hpp"
void Server::cmdPass(const std::string &args, int fd)
{
    Client *client = getClient(fd);
    if (!client)
        return;
    std::string nick = client->getNick().empty() ? "*" : client->getNick();
    std::istringstream iss(args);
    std::string pass;
    if (!(iss >> pass) || pass.empty())
    {
        ErrorReply::sendNeedMoreParams(fd, nick, "PASS");
        return;
    }
    if (client->isAuth())
    {
        ErrorReply::sendAlreadyRegistered(fd, nick);
        return;
    }
    if (pass != _password)
    {
        ErrorReply::sendPasswordMismatch(fd, nick);
        clearClients(fd);
        close(fd);
        return;
    }
    client->setAuth(true);
}
