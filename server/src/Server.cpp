#include "Server.hpp"

Server::Server() : _socketFd(-1)
{}

void Server::serverInit()
{}

void Server::serverSocket()
{
    
}
void Server::newClient(){}


// accept new client
void newData(); // accept new data

void Server::signalHandler(int sigNum)
{
    std::cout << "Signal received!" << std::endl;
    _signal = true;
}


void closeFds()
{

} 

void Server::clearClients(int fd)
{
	for(size_t i = 0; i < _pollFds.size(); i++)
    {
		if (_pollFds[i].fd == fd)
			{_pollFds.erase(_pollFds.begin() + i); break;}
	}
	for(size_t i = 0; i < _clients.size(); i++)
    {
		if (_clients[i].getFd() == fd)
			{_clients.erase(_clients.begin() + i); break;}
	}

}