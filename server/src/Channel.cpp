#include "Channel.hpp"

Channel::Channel(std::string name, Client &owner)
	: name(name), topic("No topic is set"), passwd(""), limit(0)
{
	this->ops.push_back(owner);
	this->clients.push_back(owner);
	this->modes.push_back('t');
}
Channel::~Channel() {}

std::string			Channel::getName()		{ return this->name; }

std::string			Channel::getTopic()		{ return this->topic; }

void				Channel::setTopic(std::string topic)	{ this->topic = topic; }

std::string			Channel::getPasswd()	{ return this->passwd; }

void				Channel::setPasswd(std::string passwd)	{ this->passwd = passwd; }

size_t				Channel::getLimit()		{ return this->limit; }

void				Channel::setLimit(size_t limit)			{ this->limit = limit; }

bool				Channel::getTopicIsTrue()				{ return this->topicIsTrue; }

std::vector<Client>	&Channel::getClients()	{ return this->clients; }

std::vector<Client>	&Channel::getOps()		{ return this->ops; }

bool Channel::isOp(Client &op)
{
	for (std::vector<Client>::iterator it = ops.begin(); it != ops.end(); ++it)
		if (*it == op) return true;
	return false;
}

void Channel::addOp(Client &op)
{
	if (!isOp(op))
		this->ops.push_back(op);
}

void Channel::removeOp(Client &op)
{
	for (std::vector<Client>::iterator it = ops.begin(); it != ops.end(); ++it)
		if (*it == op) { ops.erase(it); break; }
}

bool Channel::isClient(Client &c)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		if (*it == c) return true;
	return false;
}

void Channel::addClient(Client &c)
{
	if (clients.size() < limit || limit == 0)
		clients.push_back(c);
	else
		c.sendReply("471", "Channel is full");
}

void Channel::removeClient(Client &c)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		if (*it == c) { clients.erase(it); break; }
}

void Channel::broadcastMsg(Client &c, std::string msg)
{
	std::string prefix = ":" + c.getNick() + "!" + c.getUser() + "@" + c.getHost();
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == c) continue;
		it->forward(prefix + msg + "\r\n");
	}
}

void Channel::broadcast(Client &c, std::string msg)
{
	std::string prefix = ":" + c.getNick() + "!" + c.getUser() + "@" + c.getHost();
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		it->forward(prefix + msg + "\r\n");
}

bool Channel::isMode(char mode)
{
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
		if (*it == mode) return true;
	return false;
}

void Channel::addMode(char mode)	{ this->modes.push_back(mode); }

void Channel::removeMode(char mode)
{
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
		if (*it == mode) { modes.erase(it); break; }
}

std::string Channel::getModes()
{
	std::string result;
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
	{
		result += *it;
		result += " ";
	}
	return result;
}

void Channel::ChannelIsTrue(char mode)
{
	for (std::vector<char>::iterator it = modes.begin(); it != modes.end(); ++it)
		if (*it == mode) return;
}