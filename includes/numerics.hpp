#ifndef NUMERICS_HPP
# define NUMERICS_HPP

# include <iostream>
# include <map>
# include "Client.hpp"

class Client;

/********* RPL Numerics *********/

std::string RPL_WELCOME(const std::string & server, const std::string & nick, const std::string & network);
std::string RPL_YOURHOST(const std::string & server, const std::string & nick, const std::string & version);
std::string RPL_CREATED(const std::string & server, const std::string & nick, const std::string & date);
std::string RPL_MYINFO(const std::string & server, const std::string & nick, const std::string & version);
std::string RPL_ISUPPORT(const std::string & server, const std::string & nick);

std::string RPL_NOTOPIC(const std::string & server, const std::string & nick, const std::string & arg);
std::string RPL_TOPIC(const std::string & server, const std::string & nick, const std::string & chan, const std::string & arg);
std::string RPL_TOPICWHOTIME(const std::string & server, const std::string & nick, const std::string & chan, const std::string & arg);

std::string MY_RPL_TOPIC(const std::string & server, const std::string & nick, const std::string & user, const std::string & chan, const std::string & arg);
std::string RPL_NAMREPLY(const std::string & server, const std::string & nick, const std::string & arg, std::map<std::string, Client> const & members);
std::string RPL_ENDOFNAMES(const std::string & server, const std::string & nick, const std::string & arg);

/********* ERR Numerics *********/

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
std::string ERR_NOSUCHCHANNEL(const std::string & server, const std::string & nick, const std::string & arg);
std::string ERR_BADCHANMASK(const std::string & server, const std::string & nick, const std::string & arg);
std::string ERR_USERONCHANNEL(const std::string & server, const std::string & nick, const std::string & arg);
std::string ERR_INVITEONLYCHAN(const std::string & server, const std::string & nick, const std::string & arg);
std::string ERR_BADCHANNELKEY(const std::string & server, const std::string & nick, const std::string & arg);
std::string ERR_NOTONCHANNEL(const std::string & server, const std::string & nick, const std::string & arg);
std::string ERR_CHANOPRIVSNEEDED(const std::string & server, const std::string & nick, const std::string & arg);
#endif