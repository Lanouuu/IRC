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

std::string ERR_NONICKNAMEGIVEN(const std::string & server, const std::string & nick, const std::string & user)
{
    std::string buf;
    if (nick.empty() && user.empty())
        buf = ":" + server + " 431 " + "*" + " :No nickname given" + "\r\n";
    else if (user.empty())
        buf = ":" + server + " 431 " + nick + " :No nickname given" + "\r\n";
    else if (nick.empty())
        buf = ":" + server + " 431 " + user + " :No nickname given" + "\r\n";
    else
        buf = ":" + server + " 431 " + nick + " :No nickname given" + "\r\n";
    return (buf);
}

std::string ERR_ERRONEUSNICKNAME(const std::string & server, const std::string & nick, const std::string & user)
{
    std::string buf;
    if (user.empty())
        buf = ":" + server + " 432 " + "* " + nick + " :Erroneus nickname" + "\r\n";
    else
        buf = ":" + server + " 432 " + user + " " + nick + " :Erroneus nickname" + "\r\n";
    return (buf);
}

std::string ERR_NICKNAMEINUSE(const std::string & server, const std::string & nick, const std::string & user)
{
    std::string buf;
    if (user.empty())
        buf = ":" + server + " 433 " + "* " + nick + " :Nickname is already in use" + "\r\n";
    else
        buf = ":" + server + " 433 " + user + " " + nick + " :Nickname is already in use" + "\r\n";
    return (buf);
}

std::string ERR_NEEDMOREPARAMS(const std::string & server, const std::string & nick, const std::string & command)
{
    std::string buf;
    buf = ":" + server + " 461 " + nick + " " + command + " :Not enough parameters" + "\r\n";
    return (buf);
}