#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include "Socket.hpp"
#include "Client.hpp"

class Server
{
private:
    int                         _port;
    Socket                      _socket;
    std::string                 _password;
    static bool                 _signal;
    std::vector<Client>         _clients;
    std::vector<struct pollfd>  _pollFds;

public:
    Server();
    ~Server();

    void    serverInit(int port, const std::string& password);
    void    serverSocket();

    void    run();
    void    newClient();
    void    receiveData(int fd);

    static void signalHandler(int sigNum);

    void    closeFds();
    void    clearClients(int fd);

    void    parseCommands(char *buf);

    const std::string&  getPassword() const;
};

#endif

