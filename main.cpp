#include "Server.hpp"

int main()
{
	Server ser;
	std::cout << "---- SERVER ----" << std::endl;
	try
    {
		signal(SIGINT, Server::signalHandler); // catch ctrl + c
		signal(SIGQUIT, Server::signalHandler); // (catch ctrl + \)
		ser.serverInit();
	}
	catch(const std::exception& e){
		ser.closeFds(); 
		std::cerr << e.what() << std::endl;
	}
	std::cout << "The server has been closed!" << std::endl;
}