#include "Server.hpp"
#include <climits>
#include <cstdlib>

static bool parsePort(const std::string &s, int &port)
{
    if (s.empty())
        return false;
    char *end = NULL;
    errno = 0;
    long n = std::strtol(s.c_str(), &end, 10);
    if (errno != 0 || *end != '\0')
        return false;
    if (n < 1024 || n > 65535)
        return false;
    if (n > INT_MAX)
        return false;
    port = static_cast<int>(n);
    return true;
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	int port;
	if (!parsePort(av[1], port))
	{
		std::cerr << "Invalid port. Use a number between 1024 and 65535." << std::endl;
		return 1;
	}
	std::string password = av[2];
	if (password.empty())
	{
		std::cerr << "Password cannot be empty." << std::endl;
		return 1;
	}

	Server ser;
	std::cout << "---- SERVER ----" << std::endl;
	try
    {
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		ser.serverInit(port, password);
	}
	catch(const std::exception& e){
		ser.closeFds();
		std::cerr << e.what() << std::endl;
		return 1;
	}
	std::cout << "The server has been closed!" << std::endl;
	return 0;
}