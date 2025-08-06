#include "numerics.hpp"

std::string RPL_WELCOME(const std::string & server, const std::string & nick, const std::string & network)
{
    std::string buf = ":" + server + " 001 " + nick + " :Welcome to the " + network + " Network, " + nick + "\r\n";
    return (buf);
}

std::string RPL_YOURHOST(const std::string & server, const std::string & nick, const std::string & version)
{
    std::string buf = ":" + server + " 002 " + nick + " :Your host is " + server + ", running version " + version + "\r\n";
    return (buf);
}

std::string RPL_CREATED(const std::string & server, const std::string & nick, const std::string & date)
{
    std::string buf = ":" + server + " 003 " + nick +  " :This server was created " + date + "\r\n";
    return (buf);
}

std::string RPL_MYINFO(const std::string & server, const std::string & nick, const std::string & version)
{
    std::string buf = ":" + server + " 004 " + nick + " " + server + " " + version + " o " + "o" + "\r\n";
    return (buf);
}

std::string RPL_ISUPPORT(const std::string & server, const std::string & nick)
{
    std::string buf = ":" + server + " 005 " + nick + " CHANTYPES=#" + " :are supported by this server" + "\r\n";
    return (buf);
}