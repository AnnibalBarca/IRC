#pragma once
#include <string>
#include <vector>
#include "Client.hpp"
#define MAX_CHANNELNAME_SIZE 10
#define MAX_KEY_SIZE 23

class Channel
{

    private:
        std::string         name;
        std::string         topic;
        std::string         passwd;
        size_t              limit;
        std::vector<int>    clientFds;
        std::vector<int>    opFds;
        std::vector<int>    invitedFds;
        std::vector<char>   modes;
        std::time_t             createdTime;
        Client *findClient(int fd, std::vector<Client> &allClients);
    public:
	    Channel();
        Channel(std::string name, Client &owner);
        ~Channel();
        std::string getName();
        std::string getTopic();
        void        setTopic(std::string topic);
        std::string getPasswd();
        void        setPasswd(std::string passwd);
        size_t      getLimit();
        void        setLimit(size_t limit);
        std::vector<int>    &getClientFds();
        std::vector<int>    &getOpFds();
        bool    isClient(Client &c);
        void    addClient(Client &c);
        void    removeClient(Client &c);
        bool    isOp(Client &op);
        void    addOp(Client &op);
        void    removeOp(Client &op);
        bool    isInvited(Client &c);
        void    addInvite(Client &c);
        void    removeInvite(Client &c);
        void    broadcastMsg(Client &sender, const std::string &msg, std::vector<Client> &allClients);
        void    broadcast(Client &sender, const std::string &msg, std::vector<Client> &allClients);
        bool        isMode(char mode);
        void        addMode(char mode);
        void        removeMode(char mode);
        std::string getModes();
        bool        isEmpty();
        std::time_t getCreationTime() const;
};
