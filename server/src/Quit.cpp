#include "Server.hpp"
#include "ErrorReplies.hpp"

void Server::cmdQuit(const std::string &args, int fd)
{
	Client *client = getClient(fd);
	if (!client)
		return;
	std::string user = client->getUser().empty() ? "*" : client->getUser();
	std::istringstream iss(args);
	std::string extra;
	if (iss >> extra)
	{
		ErrorReply::sendNeedMoreParams(fd, user, "QUIT");
		return;
	}
	client->setAuth(false);
	throw Client::disconnected;
}
