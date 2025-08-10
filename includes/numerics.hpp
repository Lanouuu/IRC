#ifndef NUMERICS_HPP
# define NUMERICS_HPP

# include <iostream>

/*********RPL Numerics*********/

std::string RPL_WELCOME(const std::string & server, const std::string & nick, const std::string & network);
std::string RPL_YOURHOST(const std::string & server, const std::string & nick, const std::string & version);
std::string RPL_CREATED(const std::string & server, const std::string & nick, const std::string & date);
std::string RPL_MYINFO(const std::string & server, const std::string & nick, const std::string & version);
std::string RPL_ISUPPORT(const std::string & server, const std::string & nick);

/*********ERR Numerics*********/

std::string ERR_NONICKNAMEGIVEN(const std::string & server, const std::string & nick, const std::string & user);
std::string ERR_ERRONEUSNICKNAME(const std::string & server, const std::string & nick, const std::string & user);
std::string ERR_NICKNAMEINUSE(const std::string & server, const std::string & nick, const std::string & user);
std::string ERR_NEEDMOREPARAMS(const std::string & server, const std::string & nick, const std::string & command);
std::string ERR_PASSWDMISMATCH(const std::string & server);
std::string ERR_ALREADYREGISTERED(const std::string & server, const std::string & nick);
std::string ERR_UNKNOWNERROR(const std::string & server, const std::string & nick, const std::string cmd, const std::string & info);
std::string ERR_UNKNOWNCOMMAND(const std::string & server, const std::string & nick, const std::string & cmd);
std::string ERR_NOORIGIN(const std::string & server, const std::string & nick);
std::string ERR_NOTREGISTERED(const std::string & server, const std::string & nick);

#endif