#include "Server.hpp"
#include "Channel.hpp"

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
            _clients[i].disconnect();
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < _channels.size(); )
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
        if (client)
        {
            std::string nick = client->getNick().empty() ? "*" : client->getNick();
            std::string err = "421 " + nick + " " + command + ERR_UNKNOWNCOMMAND;
            send(fd, err.c_str(), err.size(), 0);
        }
    }
}

static std::string nickOr(Client *c)
{
    if (!c || c->getNick().empty())
        return "*";
    return c->getNick();
}

void Server::cmdPass(const std::string &args, int fd)
{
    Client *client = getClient(fd);
    if (!client)
        return;
    std::string nick = nickOr(client);
    std::istringstream iss(args);
    std::string pass;
    if (!(iss >> pass) || pass.empty())
    {
        std::string err = "461 " + nick + " PASS" + ERR_NEEDMOREPARAMS;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (client->isAuth())
    {
        std::string err = "462 " + nick + ERR_ALREADYREGISTERED;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (pass != _password)
    {
        std::string err = "464 " + nick + ERR_PASSWDMISMATCH;
        send(fd, err.c_str(), err.size(), 0);
        clearClients(fd);
        close(fd);
        return;
    }
    client->setAuth(true);
}

void Server::cmdKick(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = nickOr(sender);
    std::istringstream iss(args);
    std::string chanName, targetNick, reason;
    if (!(iss >> chanName >> targetNick))
    {
        std::string err = "461 " + nick + " KICK" + ERR_NEEDMOREPARAMS;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);
    if (reason.empty())
        reason = nick;
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        std::string err = "403 " + nick + " " + chanName + ERR_NOSUCHCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->isClient(*sender))
    {
        std::string err = "442 " + nick + " " + chanName + ERR_NOTONCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->isOp(*sender))
    {
        std::string err = "482 " + nick + " " + chanName + ERR_CHANOPRIVSNEEDED;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client *target = getClientByNick(targetNick);
    if (!target)
    {
        std::string err = "401 " + nick + " " + targetNick + ERR_NOSUCHNICK;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->isClient(*target))
    {
        std::string err = "441 " + nick + " " + targetNick + " " + chanName + ERR_USERNOTINCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::string kickMsg = " KICK " + chanName + " " + targetNick + " :" + reason;
    channel->broadcast(*sender, kickMsg, _clients);
    channel->removeClient(*target);
    target->removeChan(channel);
    if (channel->isEmpty())
    {
        for (size_t i = 0; i < _channels.size(); i++)
        {
            if (&_channels[i] == channel)
            {
                _channels.erase(_channels.begin() + i);
                break;
            }
        }
    }
}

void Server::cmdTopic(const std::string &args, int fd)  
{ 
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = nickOr(sender);
    std::istringstream iss(args);
    std::string chanName;
    if (!(iss >> chanName))
    {
        std::string err = "461 " + nick + " TOPIC" + ERR_NEEDMOREPARAMS;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        std::string err = "403 " + nick + " " + chanName + ERR_NOSUCHCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->isClient(*sender))
    {
        std::string err = "442 " + nick + " " + chanName + ERR_NOTONCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::string rest;
    std::getline(iss, rest);
    if (!rest.empty() && rest[0] == ' ')
        rest.erase(0, 1);
    if (rest.empty())
    {
        if (channel->getTopic().empty())
        {
            std::string rpl = "331 " + nick + " " + chanName + ERR_RPL_NOTOPIC;
            send(fd, rpl.c_str(), rpl.size(), 0);
        }
        else
        {
            std::string rpl = "332 " + nick + " " + chanName + " :" + channel->getTopic() + "\r\n";
            send(fd, rpl.c_str(), rpl.size(), 0);
        }
        return;

    }
    if (channel->isMode('t') && !channel->isOp(*sender))
    {
        std::string err = "482 " + nick + " " + chanName + ERR_CHANOPRIVSNEEDED;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    std::string initTopic = rest;
    if (initTopic[0] == ':')
        initTopic.erase(0, 1);
    channel->setTopic(initTopic);
    std::string topicMsg = ":" + nick + " TOPIC " + chanName + " :" + initTopic + "\r\n";
    channel->broadcast(*sender, topicMsg, _clients);
}

void Server::cmdInvite(const std::string &args, int fd) 
{

    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string nick = nickOr(sender);
    std::istringstream iss(args);
    std::string targetNick, chanName;
    if (!(iss >> targetNick >> chanName))
    {
        std::string err = "461 " + nick + " INVITE" + ERR_NEEDMOREPARAMS;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        std::string err = "403 " + nick + " " + chanName + ERR_NOSUCHCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (!channel->isClient(*sender))
    {
        std::string err = "442 " + nick + " " + chanName + ERR_NOTONCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (channel->isMode('i') && !channel->isOp(*sender))
    {
        std::string err = "482 " + nick + " " + chanName + ERR_CHANOPRIVSNEEDED;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    Client *target = getClientByNick(targetNick);
    if (!target)
    {
        std::string err = "401 " + nick + " " + targetNick + ERR_NOSUCHNICK;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    if (channel->isClient(*target))
    {
        std::string err = "443 " + nick + " " + targetNick + " " + chanName + ERR_USERONCHANNEL;
        send(fd, err.c_str(), err.size(), 0);
        return;
    }
    channel->addInvite(*target);
    std::string rpl = "341 " + nick + " " + targetNick + " " + chanName + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
    std::string inviteMsg = ":" + nick + " INVITE " + targetNick + " " + chanName + "\r\n";
    send(target->getFd(), inviteMsg.c_str(), inviteMsg.size(), 0);   
    

}

void Server::cmdMode(const std::string &args, int fd)   { (void)args; (void)fd; }
{}

void Server::cmdNick(const std::string &args, int fd)   { (void)args; (void)fd; }
void Server::cmdUser(const std::string &args, int fd)   { (void)args; (void)fd; }
void Server::cmdJoin(const std::string &args, int fd)   { (void)args; (void)fd; }


// --To get a client by fd: getClient(fd)
// --by Nickname : getClientByNick(Nick)
// --To get a channel: getChannel(name)
// ---- For Channel membership tests :
// --------if (!chan->isClient(*sender))
// --------if (!chan->isClient(*target))
// --------if (!chan->isOp(*sender))
// --------if (chan->isMode('i') && !chan->isInvited(*sender)) = Channel is invite-only and user not invited
// ---- For Chennel mutation :
// --------setters from Channel.hpp
//




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