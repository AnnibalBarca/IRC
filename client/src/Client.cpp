#include <Server.hpp>
#include <Client.hpp>
#include <algorithm>

std::runtime_error Client::disconnected = std::runtime_error("Client disconnected");

Client::Client(int fd, std::string ip, std::string host)
	: nick(""), user(""), ip(ip), host(host), chans(), buf(""), fd(fd), auth(false) {}
Client::~Client() {}

void		Client::setIp(std::string ip)		{ this->ip = ip; }

std::string	Client::getIp()						{ return this->ip; }

void		Client::setFd(int fd)				{ this->fd = fd; }

int			Client::getFd() const				{ return this->fd; }

void		Client::setAuth(bool auth)			{ this->auth = auth; }

bool		Client::getAuth()					{ return this->auth; }

std::string	Client::getNick()					{ return this->nick; }

void		Client::setNick(std::string nick)	{ this->nick = nick; }

std::string	Client::getUser()					{ return this->user; }

void		Client::setUser(std::string user)	{ this->user = user; }

std::string	Client::getHost()					{ return this->host; }

void		Client::setHost(std::string host)	{ this->host = host; }

bool		Client::isAuth()					{ return this->auth; }

void		Client::setAuthentified()			{ this->auth = true; }

bool		Client::isNamed()					{ return (!this->nick.empty() && !this->user.empty()); }

void		Client::setBuf(std::string buf)		{ this->buf = buf; }

void		Client::addBuf(std::string buf)		{ this->buf += buf; }

std::string	Client::getBuf()					{ return this->buf; }

std::vector<Channel *> &Client::getChans()		{ return this->chans; }

bool		Client::operator==(const Client &c) const { return this->getFd() == c.getFd(); }

void Client::forward(std::string msg)
{
	if (this->isConnected(this->fd))
		send(this->fd, msg.c_str(), msg.size(), 0);
}

void Client::sendMsg(std::string msg, Channel &ch)	{ ch.broadcast(*this, msg); }

void Client::sendMsg(std::string msg, Client &c)	{ c.forward(msg); }

void Client::sendReply(std::string code, std::string msg)
{
	this->forward(":" + this->host + " " + code + " " + msg + "\r\n");
}

bool Client::isConnected(int fd)
{
	char tmp;
	int r = recv(fd, &tmp, 1, MSG_PEEK | MSG_DONTWAIT);
	if (r == 0)  return false;
	if (r < 0)   return (errno == EAGAIN || errno == EWOULDBLOCK);
	return true;
}

void Client::addChan(Channel *chan)		{ this->chans.push_back(chan); }
void Client::removeChan(Channel *chan)
{
	for (auto it = this->chans.begin(); it != this->chans.end(); ++it)
	{
		if (*it == chan) { this->chans.erase(it); break; }
	}
}

void Client::disconnect()
{
	std::vector<Client *> to_notify;
	std::vector<Client *> notified;

	for (auto it = this->chans.begin(); it != this->chans.end(); ++it)
	{
		for (auto c = (*it)->getClients().begin(); c != (*it)->getClients().end(); ++c)
			if (&(*c) != this)
				to_notify.push_back(&(*c));
		(*it)->removeClient(*this);
		(*it)->removeOp(*this);
	}
	std::string quit_msg = ":" + this->nick + "!" + this->user + "@" + this->host + " QUIT :Leaving\r\n";
	for (auto it = to_notify.begin(); it != to_notify.end(); ++it)
	{
		if (std::find(notified.begin(), notified.end(), *it) == notified.end())
		{
			(*it)->forward(quit_msg);
			notified.push_back(*it);
		}
	}
}