#pragma once
#include <string>

namespace ErrorReply
{
    void sendUnknownCommand(int fd, const std::string &user, const std::string &command);
    void sendNotRegistered(int fd, const std::string &user);
    void sendNeedMoreParams(int fd, const std::string &user, const std::string &command);
    void sendAlreadyRegistered(int fd, const std::string &user);
    void sendPasswordMismatch(int fd, const std::string &user);
    void sendNoSuchNick(int fd, const std::string &user, const std::string &targetNick);
    void sendNoSuchChannel(int fd, const std::string &user, const std::string &chanName);
    void sendUserNotInChannel(int fd, const std::string &user, const std::string &targetNick, const std::string &chanName);
    void sendNotOnChannel(int fd, const std::string &user, const std::string &chanName);
    void sendChanOpPrivsNeeded(int fd, const std::string &user, const std::string &chanName);
    void sendUserOnChannel(int fd, const std::string &user, const std::string &targetNick, const std::string &chanName);
    void sendNoTopic(int fd, const std::string &user, const std::string &chanName);
    void sendChannelIsFull(int fd, const std::string &user, const std::string &chanName);
    void sendInviteOnlyChan(int fd, const std::string &user, const std::string &chanName);
    void sendBadChannelKey(int fd, const std::string &user, const std::string &chanName);
    void sendUnknownModeChar(int fd, const std::string &user, char mode);
    void sendNickNameInUse(int fd, const std::string &user, const std::string &nickName);
    void sendNoNickNameGiven(int fd, const std::string &user);
    void sendErroneusNickname(int fd, const std::string &user, const std::string &nickName);
    void sendCannotSendToChan(int fd, const std::string &user, const std::string &chanName);
    void sendNoRecipient(int fd, const std::string &user);
    void sendNoTextToSend(int fd, const std::string &user);
}
