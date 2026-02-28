#include "Server.hpp"

bool Server::_signal = false;

Server::Server() : _port(0)
{
}

Server::~Server()
{
    closeFds();
}

void Server::serverInit(int port, const std::string &password)
{
    _port = port;
    _password = password;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    serverSocket();
    std::cout << "Server listening on port " << _port
              << " (fd=" << _socket.getFd() << ")" << std::endl;
    run();
    closeFds();
    std::cout << "Server shut down." << std::endl;
}

void Server::serverSocket()
{
    _socket.create();
    _socket.setReuseAddr();
    _socket.bindTo(_port);
    _socket.startListening();
    _socket.setNonBlocking();
    struct pollfd pfd;
    pfd.fd = _socket.getFd();
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);
}

void Server::run()
{
    while (!_signal)
    {
        if (_pollFds.empty())
            break;
        int ret = poll(&_pollFds[0], (nfds_t)_pollFds.size(), -1);
        if (ret < 0)
        {
            if (_signal)
                break;
            throw std::runtime_error(std::string("poll() failed: ") + strerror(errno));
        }
        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].revents == 0)
                continue;
            if (_pollFds[i].fd == _socket.getFd() && (_pollFds[i].revents & POLLIN))
            {
                newClient();
                break;
            }
            int fd = _pollFds[i].fd;
            if (_pollFds[i].revents & (POLLHUP | POLLERR))
            {
                std::cout << "Client <" << fd << "> disconnected." << std::endl;
                clearClients(fd);
                close(fd);
                break;
            }
            if (_pollFds[i].revents & POLLIN)
            {
                receiveData(fd);
                break;
            }
        }
    }
}

void Server::newClient()
{
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(_socket.getFd(), (struct sockaddr *)&client_addr, &len);
    if (client_fd < 0)
        return;
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        close(client_fd);
        return;
    }
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);
    Client client;
    client.setFd(client_fd);
    client.setIp(inet_ntoa(client_addr.sin_addr));
    _clients.push_back(client);
    std::cout << "Client <" << client_fd << "> connected from "
              << inet_ntoa(client_addr.sin_addr) << std::endl;
}

void Server::receiveData(int fd)
{
    char buf[512];
    ssize_t bytes = recv(fd, buf, sizeof(buf) - 1, 0);
    if (bytes <= 0)
    {
        if (bytes == 0)
            std::cout << "Client <" << fd << "> disconnected." << std::endl;
        else
            std::cerr << "Client <" << fd << "> recv error: " << strerror(errno) << std::endl;
        clearClients(fd);
        close(fd);
        return;
    }
    buf[bytes] = '\0';
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() != fd)
            continue;
        _clients[i].addBuf(std::string(buf, bytes));
        std::string &rbuf = _clients[i].getBuf();
        size_t pos;
        while ((pos = rbuf.find('\n')) != std::string::npos)
        {
            std::string cmd = rbuf.substr(0, pos);
            rbuf.erase(0, pos + 1);
            if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
                cmd.erase(cmd.size() - 1);
            if (cmd.empty())
                continue;
            try
            {
                parseCommands(cmd, fd);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Parse error: " << e.what() << std::endl;
            }
        }
        break;
    }
}

void Server::signalHandler(int sigNum)
{
    (void)sigNum;
    std::cout << "\nSignal received – shutting down." << std::endl;
    _signal = true;
}

void Server::closeFds()
{
    for (size_t i = 0; i < _clients.size(); i++)
        close(_clients[i].getFd());
    _clients.clear();
    _pollFds.clear();
    _socket.closeSocket();
}

void Server::clearClients(int fd)
{
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}

const std::string &Server::getPassword() const
{
    return _password;
}

void Server::parseCommands(const std::string &cmd, int fd)
{
    if (cmd.empty())
        return;
    std::istringstream iss(cmd);
    std::string command;
    std::string args;
    iss >> command;
    std::getline(iss, args);
    if (!args.empty() && args[0] == ' ')
        args.erase(0, 1);
    for (size_t i = 0; i < command.size(); i++)
        command[i] = std::toupper(command[i]);
    typedef void (Server::*CmdHandling)(const std::string &, int);
    std::map<std::string, CmdHandling> cmds;
    cmds["PASS"] = &Server::cmdPass;
    cmds["NICK"] = &Server::cmdNick;
    cmds["USER"] = &Server::cmdUser;
    cmds["JOIN"] = &Server::cmdJoin;
    cmds["KICK"] = &Server::cmdKick;
    cmds["INVITE"] = &Server::cmdInvite;
    cmds["TOPIC"] = &Server::cmdTopic;
    cmds["MODE"] = &Server::cmdMode;
    std::map<std::string, CmdHandling>::iterator iter = cmds.find(command);
    if (iter != cmds.end())
        (this->*(iter->second))(args, fd);
    else
    {
        Client *client = getClient(fd);
        if (client && client->isConnected(fd))
        {
            std::string err = "421 " + client->getNick() + " " + command + " :Unknown command\r\n";
            send(fd, err.c_str(), err.size(), 0);
        }
    }
}

void Server::cmdPass(const std::string &args, int fd)
{
    if (args.empty())
        return;
    Client *client = getClient(fd);
    std::istringstream iss(args);
    std::string pass;
    iss >> pass;
    if (!(iss >> pass) || pass.empty())
    {
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        std::string err = "461 " + nick + " PASS" + ERR_NEEDMOREPARAMS;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    else if (!client->getNick().empty() || !client->getUser().empty())
    {
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        std::string err = "462 " + nick + ERR_ALREADYREGISTERED;
        send(fd, err.c_str(), err.size(), 0);
    }
    else if (!getPass().empty() && pass != getPass())
    {
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        std::string err = "464 " + nick + " " + ERR_PASSWDMISMATCH;
        send(fd, err.c_str(), err.size(), 0);
        clearClients(fd);
        close(fd);
        return;
    }
    setPass(pass);
}

void Server::cmdKick(const std::string &args, int fd)
{
    if (args.empty())
        return;
    std::istringstream iss(args);
    std::string name;
    Client *client = getClient(fd);
    if (!(iss >> name) || name.empty())
    {
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        std::string err = "461 " + nick + " PASS" + ERR_NEEDMOREPARAMS;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Channel *channel = getChannel(name);
    if (!channel)
    {
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        std::string err = "403 " + nick + " " + name + ERR_NOSUCHCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (!client->isOperator())
    {
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        std::string err = "482 " + nick + " " + name + ERR_CHANOPRIVSNEEDED;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if ()
    
    std::istringstream iss(args);
    std::string kick;
    iss >> kick;
    
    if (kick != )

    channel.getName
    while ()

}
void cmdTopic(const std::string &args, int fd);
void cmdInvite(const std::string &args, int fd);
void cmdMode(const std::string &args, int fd);
void cmdNick(const std::string &args, int fd);
void cmdUser(const std::string &args, int fd);
void cmdJoin(const std::string &args, int fd);

void Server::setPass(std::string &password)
{
    _password = password;
}

const std::string &Server::getPass() const
{
    return (_password);
}

Client* Server::getClient(int fd)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
            return &_clients[i];
    }
    return NULL;
}

Channel* Server::getChannel(std::string name)
{
    for (size_t i = 0; i < _channel.size(); i++)
    {
        if (_channel[i].getName() == name)
            return &_channel[i];
    }
    return NULL;
}