#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include "Client.hpp"

class Server
{
private:
    int _port;
    int _socketFd; //descripteur serveyr
    static bool _signal; //signal ou pas
    std::vector<Client> _clients;
    std::vector<struct pollfd> _pollFds;
public:
    Server();

    void serverInit();
    void serverSocket(); // server socket creation
    void newClient(); // accept new client
    void newData(); // accept new data

    static void signalHandler(int sigNum);

    void closeFds(); 
    void clearClients(int fd);
};

