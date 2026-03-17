#include "ErrorReplies.hpp"
#include "Messages.hpp"
#include <sys/socket.h>
namespace ErrorReply
{
    static void sendReply(int fd, const std::string &msg)
    {
        send(fd, msg.c_str(), msg.size(), 0);
    }
    void sendUnknownCommand(int fd, const std::string &nick, const std::string &command)
    {
        sendReply(fd, "421 " + nick + " " + command + ERR_UNKNOWNCOMMAND);
    }
    void sendNeedMoreParams(int fd, const std::string &nick, const std::string &command)
    {
        sendReply(fd, "461 " + nick + " " + command + ERR_NEEDMOREPARAMS);
    }
    void sendAlreadyRegistered(int fd, const std::string &nick)
    {
        sendReply(fd, "462 " + nick + ERR_ALREADYREGISTERED);
    }
    void sendPasswordMismatch(int fd, const std::string &nick)
    {
        sendReply(fd, "464 " + nick + ERR_PASSWDMISMATCH);
    }
    void sendNoSuchNick(int fd, const std::string &nick, const std::string &targetNick)
    {
        sendReply(fd, "401 " + nick + " " + targetNick + ERR_NOSUCHNICK);
    }
    void sendNoSuchChannel(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "403 " + nick + " " + chanName + ERR_NOSUCHCHANNEL);
    }
    void sendUserNotInChannel(int fd, const std::string &nick, const std::string &targetNick, const std::string &chanName)
    {
        sendReply(fd, "441 " + nick + " " + targetNick + " " + chanName + ERR_USERNOTINCHANNEL);
    }
    void sendNotOnChannel(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "442 " + nick + " " + chanName + ERR_NOTONCHANNEL);
    }
    void sendChanOpPrivsNeeded(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "482 " + nick + " " + chanName + ERR_CHANOPRIVSNEEDED);
    }
    void sendUserOnChannel(int fd, const std::string &nick, const std::string &targetNick, const std::string &chanName)
    {
        sendReply(fd, "443 " + nick + " " + targetNick + " " + chanName + ERR_USERONCHANNEL);
    }
    void sendNoTopic(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "331 " + nick + " " + chanName + ERR_RPL_NOTOPIC);
    }
    void sendChannelIsFull(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "471 " + nick + " " + chanName + ERR_CHANNELISFULL);
    }
    void sendInviteOnlyChan(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "473 " + nick + " " + chanName + ERR_INVITEONLYCHAN);
    }
    void sendBadChannelKey(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, "475 " + nick + " " + chanName + ERR_BADCHANNELKEY);
    }
    void sendUnknownModeChar(int fd, const std::string &nick, char mode)
    {
        sendReply(fd, "472 " + nick + " " + std::string(1, mode) + ERR_UNKNOWNMODE);
    }
}
