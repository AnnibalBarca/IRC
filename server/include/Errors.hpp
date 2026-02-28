#ifndef ERRORS_HPP
#define ERRORS_HPP

#include "Client.hpp"
#include "Channel.hpp"

#define ERR_PASSWDMISMATCH = " :Password incorrect\r\n"
#define ERR_NEEDMOREPARAMS = " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTERED = " :You may not reregister\r\n"
#define ERR_NOSUCHCHANNEL = " :No such channel\r\n"
#define ERR_CHANNOPRIVSNEEDED = " :You're not channel operator\r\n"
#define ERR_USERNOTINCHANNEL = " :They aren't on that channel\r\n"

#endif