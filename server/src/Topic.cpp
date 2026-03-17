#include "Server.hpp"
#include "Channel.hpp"
#include "ErrorReplies.hpp"
#include "SuccessReplies.hpp"

void Server::cmdTopic(const std::string &args, int fd)
{
    Client *sender = getClient(fd);
    if (!sender)
        return;
    std::string user = sender->getUser().empty() ? "*" : sender->getUser();
    std::istringstream iss(args);
    std::string chanName;
    if (!(iss >> chanName))
    {
        ErrorReply::sendNeedMoreParams(fd, user, "TOPIC");
        return;
    }
    Channel *channel = getChannel(chanName);
    if (!channel)
    {
        ErrorReply::sendNoSuchChannel(fd, user, chanName);
        return;
    }
    if (!channel->isClient(*sender))
    {
        ErrorReply::sendNotOnChannel(fd, user, chanName);
        return;
    }
    std::string rest;
    std::getline(iss, rest);
    if (!rest.empty() && rest[0] == ' ')
        rest.erase(0, 1);
    if (rest.empty())
    {
        if (channel->getTopic().empty())
            ErrorReply::sendNoTopic(fd, user, chanName);
        else
        {
            std::string rpl = "332 " + user + " " + chanName + " :" + channel->getTopic() + "\r\n";
            send(fd, rpl.c_str(), rpl.size(), 0);
        }
        return;
    }
    if (channel->isMode('t') && !channel->isOp(*sender))
    {
        ErrorReply::sendChanOpPrivsNeeded(fd, user, chanName);
        return;
    }
    std::string initTopic = rest;
    if (initTopic[0] == ':')
        initTopic.erase(0, 1);
    channel->setTopic(initTopic);
    std::string topicMsg = " TOPIC " + chanName + " :" + initTopic;
    channel->broadcast(*sender, topicMsg, _clients);
    SuccessReply::sendTopicUpdated(fd, user, chanName);
}
