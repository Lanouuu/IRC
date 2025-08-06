#ifndef NUMERICS_HPP
# define NUMERICS_HPP

# include <iostream>

std::string RPL_WELCOME(const std::string & server, const std::string & nick, const std::string & network);
std::string RPL_YOURHOST(const std::string & server, const std::string & nick, const std::string & version);
std::string RPL_CREATED(const std::string & server, const std::string & nick, const std::string & date);
std::string RPL_MYINFO(const std::string & server, const std::string & nick, const std::string & version);
std::string RPL_ISUPPORT(const std::string & server, const std::string & nick);

#endif