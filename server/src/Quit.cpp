#include "Server.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

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
	SuccessReply::sendQuitConfirmed(fd, user);
	client->setAuth(false);
	throw Client::disconnected;
}
