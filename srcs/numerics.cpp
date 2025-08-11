#include "numerics.hpp"


/****************************************************************************/
/*                              RPL Numerics                                */
/****************************************************************************/


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
    std::string buf = ":" + server + " 005 " + nick + " CHANTYPES=# NICKLEN=9 USERLEN=12" + " :are supported by this server" + "\r\n";
    return (buf);
}

std::string RPL_NOTOPIC(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf = ":" + server + " 331 " + nick + " " + arg + " :No topic is set" + "\r\n";
    return (buf); 
}

std::string RPL_TOPIC(const std::string & server, const std::string & nick, const std::string & chan, const std::string & arg)
{
    std::string buf = ":" + server + " 332 " + nick + " " + chan + " :" + arg + "\r\n";
    return (buf); 
}

std::string RPL_TOPICWHOTIME(const std::string & server, const std::string & nick, const std::string & chan, const std::string & arg)
{
    std::string buf = ":" + server + " 333 " + nick + " " + chan + " :" + arg + "\r\n";
    return (buf); 
}

std::string MY_RPL_TOPIC(const std::string & server, const std::string & nick, const std::string & user, const std::string & chan, const std::string & arg)
{
    std::string buf = ":" + nick + "!" + user + "@" + server + " TOPIC " + chan + " :" + arg + "\r\n";
    return (buf); 
}


std::string RPL_NAMREPLY(const std::string & server, const std::string & nick, const std::string & arg, std::map<std::string, Client> const & members)
{
    std::string buf;
    
    buf = ":" + server + " 353 " + nick + " = " + arg + " :";
    for (std::map<std::string, Client>::const_iterator it = members.begin(); it != members.end(); it++)
    {
        buf += it->first + " ";
    }
    buf += "\r\n";
    return (buf); 
}

std::string RPL_ENDOFNAMES(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 366 " + nick + " " + arg + " :End of /NAMES list" + "\r\n";
    return (buf);  
}
std::string ERR_CHANOPRIVSNEEDED(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf = ":" + server + " 482 " + nick + " " + arg + " :You're not channel operator" + "\r\n";
    return (buf); 
}

/****************************************************************************/
/*                              ERR Numerics                                */
/****************************************************************************/


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
    if (nick.empty())
        buf = ":" + server + " 461 " + "* " + command + " :Not enough parameters" + "\r\n";
    else
        buf = ":" + server + " 461 " + nick + " " + command + " :Not enough parameters" + "\r\n";
    return (buf);
}

std::string ERR_PASSWDMISMATCH(const std::string & server)
{
    std::string buf = ":" + server + " 464 " + "*" + " :Password incorrect" + "\r\n";
    return (buf);
}

std::string ERR_ALREADYREGISTERED(const std::string & server, const std::string & nick)
{
    std::string buf;
    if (nick.empty())
        buf = ":" + server + " 462 " + "*" + " :You may not reregister" + "\r\n";
    else
        buf = ":" + server + " 462 " + nick + " :You may not reregister" + "\r\n";
    return (buf);
}

std::string ERR_UNKNOWNERROR(const std::string & server, const std::string & nick, const std::string cmd, const std::string & info)
{
    std::string buf;
    if (cmd.empty() && nick.empty())
        buf = ":" + server + " 400 " + "* " + "* " + ":" + info + "\r\n";
    else if (nick.empty())
        buf = ":" + server + " 400 " + "* " + cmd + " " + ":" + info + "\r\n";
    else if (cmd.empty())
        buf = ":" + server + " 400 " + nick + " " + "* " + ":" + info + "\r\n";
    return (buf);
}

std::string ERR_UNKNOWNCOMMAND(const std::string & server, const std::string & nick, const std::string & cmd)
{
    std::string buf;
    if (nick.empty())
        buf = ":" + server + " 421 " + "* " + cmd + " :Unknown command" + "\r\n";
    else
        buf = buf = ":" + server + " 421 " + nick + " " + cmd + " :Unknown command" + "\r\n";
    return (buf);
}

std::string ERR_NOORIGIN(const std::string & server, const std::string & nick)
{
    std::string buf;
    if (nick.empty())
        buf = ":" + server + " 409 " + "*" + " :No origin specified" + "\r\n";
    else
        buf = ":" + server + " 409 " + nick + " :No origin specified" + "\r\n";
    return (buf);
}

std::string ERR_NOTREGISTERED(const std::string & server, const std::string & nick)
{
    std::string buf;
    if (nick.empty())
        buf = ":" + server + " 451 " + "*" + " :You have not registered" + "\r\n";
    else
        buf = buf = ":" + server + " 451 " + nick + " :You have not registered" + "\r\n";
    return (buf);
}

std::string ERR_NOSUCHCHANNEL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 403 " + nick + " " + arg + " :No such channel" + "\r\n";
    return (buf);
}

std::string ERR_BADCHANMASK(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 476 " + nick + " " + arg + " :Bad Channel Mask" + "\r\n";
    return (buf);   
}

std::string ERR_USERONCHANNEL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 443 " + nick + " " + arg + " :is already on channel" + "\r\n";
    return (buf);  
}

std::string ERR_INVITEONLYCHAN(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 473 " + nick + " " + arg + " :Cannot join channel (+i)" + "\r\n";
    return (buf);  
}

std::string ERR_BADCHANNELKEY(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 475 " + nick + " " + arg + " :Cannot join channel (+k)" + "\r\n";
    return (buf); 
}

std::string ERR_NOTONCHANNEL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = ":" + server + " 442 " + nick + " " + arg + "<client> <channel> :You're not on that channel" + "\r\n";
    return (buf); 
}

