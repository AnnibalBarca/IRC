#include "SuccessReplies.hpp"
#include <sys/socket.h>

namespace SuccessReply
{
    static void sendReply(int fd, const std::string &msg)
    {
        send(fd, msg.c_str(), msg.size(), 0);
    }

    void sendPassAccepted(int fd, const std::string &nick)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :PASS accepted\r\n");
    }

    void sendNickChanged(int fd, const std::string &oldNick, const std::string &newNick)
    {
        std::string prefix = oldNick.empty() ? newNick : oldNick;
        sendReply(fd, ":" + prefix + " NICK " + newNick + "\r\n");
    }

    void sendUserSet(int fd, const std::string &nick)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :USER accepted\r\n");
    }

    void sendJoinConfirmed(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :JOIN success " + chanName + "\r\n");
    }

    void sendInviteConfirmed(int fd, const std::string &nick, const std::string &targetNick, const std::string &chanName)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :INVITE sent to " + targetNick + " for " + chanName + "\r\n");
    }

    void sendKickConfirmed(int fd, const std::string &nick, const std::string &targetNick, const std::string &chanName)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :KICK success " + targetNick + " from " + chanName + "\r\n");
    }

    void sendTopicUpdated(int fd, const std::string &nick, const std::string &chanName)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :TOPIC updated for " + chanName + "\r\n");
    }

    void sendModeUpdated(int fd, const std::string &nick, const std::string &chanName, const std::string &modes)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :MODE success " + chanName + " " + modes + "\r\n");
    }

    void sendQuitConfirmed(int fd, const std::string &nick)
    {
        sendReply(fd, ":irc NOTICE " + nick + " :QUIT accepted\r\n");
    }
}
