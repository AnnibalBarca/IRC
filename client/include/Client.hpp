#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include <vector>
#include <string>
#include <stdexcept>

class Channel;
class Client
{
private:
	std::string		nick;
	std::string		user;
	std::string		ip;
	std::string		host;
	std::vector<Channel *> chans;
	std::string		buf;
	int				fd;
	bool			auth;

public:
	Client(int fd, std::string ip, std::string host);
	~Client();

	char mode;
	static std::runtime_error disconnected;

	void		setIp(std::string ip);
	std::string	getIp();
	void		setFd(int fd);
	int			getFd() const;
	void		setAuth(bool auth);
	bool		getAuth();
	std::string	getNick();
	void		setNick(std::string nick);
	std::string	getUser();
	void		setUser(std::string user);
	std::string	getHost();
	void		setHost(std::string host);
	bool		isAuth();
	void		setAuthentified();
	bool		isNamed();
	void		addChan(Channel *chan);
	void		removeChan(Channel *chan);
	std::vector<Channel *> &getChans();
	void		setBuf(std::string buf);
	void		addBuf(std::string buf);
	std::string	getBuf();
	bool		isConnected(int fd);
	void		forward(std::string msg);
	bool		operator==(const Client &c) const;
	void		sendMsg(std::string msg, Channel &ch);
	void		sendMsg(std::string msg, Client &c);
	void		sendReply(std::string code, std::string msg);
	void		disconnect();
};

#endif