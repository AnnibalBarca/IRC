#include "Server.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

void Server::cmdQuit(const std::string &args, int fd)
{
	Client *client = getClient(fd);
	if (!client)
		return;
	std::string nick = client->getNick().empty() ? "*" : client->getNick();
	std::istringstream iss(args);
	std::string extra;
	if (iss >> extra)
	{
		ErrorReply::sendNeedMoreParams(fd, nick, "QUIT");
		return;
	}
	SuccessReply::sendQuitConfirmed(fd, nick);
	client->setAuth(false);
	throw std::runtime_error("Client disconnected");
}
