#include "Server.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

void Server::cmdPass(const std::string &args, int fd)
{
    Client *client = getClient(fd);
    if (!client)
        return;
    std::string user = client->getUser().empty() ? "*" : client->getUser();
    std::istringstream iss(args);
    std::string pass;
    if (!(iss >> pass) || pass.empty())
    {
        ErrorReply::sendNeedMoreParams(fd, user, "PASS");
        return;
    }
    if (client->isAuth())
    {
        ErrorReply::sendAlreadyRegistered(fd, user);
        return;
    }
    if (pass != _password)
    {
        ErrorReply::sendPasswordMismatch(fd, user);
        clearClients(fd);
        close(fd);
        return;
    }
    client->setAuth(true);
    SuccessReply::sendPassAccepted(fd, user);
}
