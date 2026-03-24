#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"
#include <cstdlib>
#include <algorithm>

bool Server::_signal = false;
Server* Server::_instance = NULL;

Server::Server() : _port(0)
{
    _instance = this;
}

Server::~Server()
{
    closeFds();
    if (_instance == this)
        _instance = NULL;
}

Server* Server::instance()
{
    return _instance;
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
        for (size_t i = 0; i < _pollFds.size();)
        {
            if (_pollFds[i].revents == 0)
            {
                i++;
                continue;
            }
            if (_pollFds[i].fd == _socket.getFd() && (_pollFds[i].revents & POLLIN))
            {
                newClient();
                i++;
                continue;
            }
            int fd = _pollFds[i].fd;
            if (_pollFds[i].revents & (POLLHUP | POLLERR))
            {
                std::cout << "Client <" << fd << "> disconnected." << std::endl;
                clearClients(fd);
                close(fd);
                continue;
            }
            if (_pollFds[i].revents & POLLIN)
            {
                receiveData(fd);
                if (!getClient(fd))
                    continue;
            }
            if (_pollFds[i].revents & POLLOUT)
            {
                flushClientOutput(fd);
                if (!getClient(fd))
                    continue;
            }
            i++;
        }
    }
}

void Server::enablePollOut(int fd)
{
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds[i].events |= POLLOUT;
            return;
        }
    }
}

void Server::disablePollOut(int fd)
{
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds[i].events &= ~POLLOUT;
            return;
        }
    }
}

void Server::queueToFd(int fd, const std::string &msg)
{
    Client *client = getClient(fd);
    if (!client)
        return;
    client->addOutBuf(msg);
    enablePollOut(fd);
}

void Server::flushClientOutput(int fd)
{
    Client *client = getClient(fd);
    if (!client)
        return;
    std::string &pending = client->getOutBuf();
    while (!pending.empty())
    {
        ssize_t sent = send(fd, pending.c_str(), pending.size(), 0);
        if (sent > 0)
        {
            pending.erase(0, static_cast<size_t>(sent));
            continue;
        }
        if (sent < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            enablePollOut(fd);
            return;
        }
        clearClients(fd);
        close(fd);
        return;
    }
    disablePollOut(fd);
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
    std::string addr = inet_ntoa(client_addr.sin_addr);
    client.setIp(addr);
    client.setHost(addr);
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
    Client *client = getClient(fd);
    if (!client)
        return;
    client->addBuf(std::string(buf, bytes));
    while (true)
    {
        client = getClient(fd);
        if (!client)
            return;
        std::string &readBuffer = client->getBuf();
        size_t newlinePos = readBuffer.find('\n');
        if (newlinePos == std::string::npos)
            break;
        std::string rawCommand = readBuffer.substr(0, newlinePos);
        readBuffer.erase(0, newlinePos + 1);
        if (!rawCommand.empty() && rawCommand[rawCommand.size() - 1] == '\r')
            rawCommand.erase(rawCommand.size() - 1);
        if (rawCommand.empty())
            continue;
        try
        {
            parseCommands(rawCommand, fd);
        }
        catch (const std::exception &e)
        {
            (void)e;
            clearClients(fd);
            close(fd);
            return;
        }
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
            Client &client = _clients[i];
            std::string quitMsg = ":" + client.getNick() + "!" + client.getUser() + "@" + client.getHost() + " QUIT :Leaving\r\n";
            std::vector<int> toNotify;
            std::vector<std::string> joinedChans = client.getChans();
            for (size_t j = 0; j < joinedChans.size(); j++)
            {
                Channel *channel = getChannel(joinedChans[j]);
                if (!channel)
                    continue;
                std::vector<int> &clientFds = channel->getClientFds();
                for (size_t k = 0; k < clientFds.size(); k++)
                {
                    if (clientFds[k] != fd)
                        toNotify.push_back(clientFds[k]);
                }
                channel->removeClient(client);
                channel->removeOp(client);
                channel->removeInvite(client);
            }
            std::vector<int> notified;
            for (size_t j = 0; j < toNotify.size(); j++)
            {
                if (std::find(notified.begin(), notified.end(), toNotify[j]) != notified.end())
                    continue;
                queueToFd(toNotify[j], quitMsg);
                notified.push_back(toNotify[j]);
            }
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < _channels.size();)
    {
        if (_channels[i].isEmpty())
            _channels.erase(_channels.begin() + i);
        else
            i++;
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
    Client *client = getClient(fd);
    if (!client)
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
    std::string nick = client->getNick().empty() ? "*" : client->getNick();
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
    cmds["QUIT"] = &Server::cmdQuit;
    cmds["PRIVMSG"] = &Server::cmdPrivMsg;
    std::map<std::string, CmdHandling>::iterator iter = cmds.find(command);
    if (iter == cmds.end())
    {
        ErrorReply::sendUnknownCommand(fd, nick, command);
        return;
    }
    if (!client->isAuth() && command != "PASS" && command != "QUIT")
    {
        ErrorReply::sendNotRegistered(fd, nick);
        return;
    }
    if (client->isAuth() && !client->isRegistered() && command != "PASS" && command != "NICK" && command != "USER" && command != "QUIT")
    {
        ErrorReply::sendNotRegistered(fd, nick);
        return;
    }
    (this->*(iter->second))(args, fd);
}

Client *Server::getClient(int fd)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
            return &_clients[i];
    }
    return NULL;
}

Client *Server::getClientByNick(const std::string &nick)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getNick() == nick)
            return &_clients[i];
    }
    return NULL;
}

Channel *Server::getChannel(const std::string &name)
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i].getName() == name)
            return &_channels[i];
    }
    return NULL;
}

std::string Server::normalizeChannelName(const std::string &name)
{
    std::string normalized = name;
    for (size_t i = 0; i < normalized.size(); i++)
        normalized[i] = std::toupper(normalized[i]);
    return normalized;
}
