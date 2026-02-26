#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <Client.hpp>
#include <iostream>
#include <vector>

class Client;

class Channel
{
private:
	std::string			name;
	std::vector<Client>	ops;
	std::vector<Client>	clients;
	std::string			passwd;
	std::string			topic;
	std::vector<char>	modes;
	size_t				limit;
	bool				topicIsTrue;

public:
	Channel(std::string name, Client &owner);
	~Channel();

	void		broadcast(Client &c, std::string msg);
	void		broadcastMsg(Client &c, std::string msg);
	std::string	getName();
	std::string	getTopic();
	void		setTopic(std::string topic);
	bool		getTopicIsTrue();
	std::vector<Client> &getClients();
	std::vector<Client> &getOps();
	void		addOp(Client &op);
	void		removeOp(Client &op);
	bool		isOp(Client &op);
	void		addClient(Client &c);
	void		removeClient(Client &c);
	bool		isClient(Client &c);
	void		setPasswd(std::string passwd);
	std::string	getPasswd();
	void		setLimit(size_t limit);
	size_t		getLimit();
	void		addMode(char mode);
	void		removeMode(char mode);
	bool		isMode(char mode);
	std::string	getModes();
	void		ChannelIsTrue(char mode);
};

#endif