#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <cerrno>
#include <ctime>

#define MAX_USERNAME_SIZE 10
#define MAX_NICKNAME_SIZE 10

class Client
{
    private:
        std::string             nick;
        std::string             user;
        std::string             ip;
        std::string             host;
        std::vector<std::string> chans;
        std::string             buf;
        std::string             outBuf;
        int                     fd;
        bool                    auth;
        bool                    welcomed;
    public:
        Client();
        Client(int fd, std::string ip, std::string host);
        ~Client();
        void        setFd(int fd);
        int         getFd() const;
        void        setAuth(bool auth);
        bool        getAuth();
        bool        isAuth();
        void        setAuthentified();
        bool        isRegistered();
        bool        isWelcomed();
        void        setWelcomed(bool val);
        std::string getNick();
        void        setNick(std::string nick);
        std::string getUser();
        void        setUser(std::string user);
        std::string getHost();
        void        setHost(std::string host);
        std::string getIp();
        void        setIp(std::string ip);
        bool        isNamed();
        void        setBuf(std::string buf);
        void        addBuf(std::string buf);
        std::string &getBuf();
        void        addOutBuf(const std::string &msg);
        std::string &getOutBuf();
        std::vector<std::string> &getChans();
        void        addChan(const std::string &chanName);
        void        removeChan(const std::string &chanName);
        void        forward(std::string msg);
        void        sendMsg(std::string msg, Client &c);
        void        sendReply(std::string code, std::string msg);
        bool        operator==(const Client &c) const;
};
