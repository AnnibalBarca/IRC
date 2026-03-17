#include "Server.hpp"
#include "ErrorReplies.hpp"
void Server::cmdQuit(const std::string &args, int fd)
{
	Client *client = getClient(fd);
	if (!client)
		return;
	std::istringstream iss(args);
	std::string extra;
	if (iss >> extra)
	{
		std::string nick = client->getNick().empty() ? "*" : client->getNick();
		ErrorReply::sendNeedMoreParams(fd, nick, "QUIT");
		return;
	}
	client->setAuth(false);
	throw Client::disconnected;
}
