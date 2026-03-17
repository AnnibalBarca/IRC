#include "ErrorReplies.hpp"
#include "Messages.hpp"
#include <sys/socket.h>
namespace ErrorReply
{
    static void sendReply(int fd, const std::string &msg)
    {
        send(fd, msg.c_str(), msg.size(), 0);
    }
    void sendUnknownCommand(int fd, const std::string &user, const std::string &command)
    {
        sendReply(fd, "421 " + user + " " + command + ERR_UNKNOWNCOMMAND);
    }
    void sendNeedMoreParams(int fd, const std::string &user, const std::string &command)
    {
        sendReply(fd, "461 " + user + " " + command + ERR_NEEDMOREPARAMS);
    }
    void sendAlreadyRegistered(int fd, const std::string &user)
    {
        sendReply(fd, "462 " + user + ERR_ALREADYREGISTERED);
    }
    void sendPasswordMismatch(int fd, const std::string &user)
    {
        sendReply(fd, "464 " + user + ERR_PASSWDMISMATCH);
    }
    void sendNoSuchNick(int fd, const std::string &user, const std::string &targetNick)
    {
        sendReply(fd, "401 " + user + " " + targetNick + ERR_NOSUCHNICK);
    }
    void sendNoSuchChannel(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "403 " + user + " " + chanName + ERR_NOSUCHCHANNEL);
    }
    void sendUserNotInChannel(int fd, const std::string &user, const std::string &targetNick, const std::string &chanName)
    {
        sendReply(fd, "441 " + user + " " + targetNick + " " + chanName + ERR_USERNOTINCHANNEL);
    }
    void sendNotOnChannel(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "442 " + user + " " + chanName + ERR_NOTONCHANNEL);
    }
    void sendChanOpPrivsNeeded(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "482 " + user + " " + chanName + ERR_CHANOPRIVSNEEDED);
    }
    void sendUserOnChannel(int fd, const std::string &user, const std::string &targetNick, const std::string &chanName)
    {
        sendReply(fd, "443 " + user + " " + targetNick + " " + chanName + ERR_USERONCHANNEL);
    }
    void sendNoTopic(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "331 " + user + " " + chanName + ERR_RPL_NOTOPIC);
    }
    void sendChannelIsFull(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "471 " + user + " " + chanName + ERR_CHANNELISFULL);
    }
    void sendInviteOnlyChan(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "473 " + user + " " + chanName + ERR_INVITEONLYCHAN);
    }
    void sendBadChannelKey(int fd, const std::string &user, const std::string &chanName)
    {
        sendReply(fd, "475 " + user + " " + chanName + ERR_BADCHANNELKEY);
    }
    void sendUnknownModeChar(int fd, const std::string &user, char mode)
    {
        sendReply(fd, "472 " + user + " " + std::string(1, mode) + ERR_UNKNOWNMODE);
    }
    void sendNickNameInUse(int fd, const std::string &user)
    {
        sendReply(fd, "433 " + user + ERR_NICKNAMEINUSE);
    }
}
