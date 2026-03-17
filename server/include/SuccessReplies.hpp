#pragma once
#include <string>

namespace SuccessReply
{
    void sendPassAccepted(int fd, const std::string &nick);
    void sendNickChanged(int fd, const std::string &oldNick, const std::string &newNick);
    void sendUserSet(int fd, const std::string &nick);
    void sendJoinConfirmed(int fd, const std::string &nick, const std::string &chanName);
    void sendInviteConfirmed(int fd, const std::string &nick, const std::string &targetNick, const std::string &chanName);
    void sendKickConfirmed(int fd, const std::string &nick, const std::string &targetNick, const std::string &chanName);
    void sendTopicUpdated(int fd, const std::string &nick, const std::string &chanName);
    void sendModeUpdated(int fd, const std::string &nick, const std::string &chanName, const std::string &modes);
    void sendQuitConfirmed(int fd, const std::string &nick);
}
