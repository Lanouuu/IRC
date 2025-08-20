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

std::string RPL_NAMREPLY(const std::string & server, const std::string & nick, Channel const & channel)
{
    std::string buf;
    
    buf = ":" + server + " 353 " + nick + " = " + channel.getName() + " :";
    for (std::map<std::string, Client>::const_iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); it++)
    {
        if (channel.isOperator(it->first))
            buf += "@";
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

std::string RPL_PART(const std::string & server, const std::string & nick, const std::string & user, const std::string & chan, const std::string & reason)
{
    std::string buf;
    if(!reason.empty())
        buf = ":" + nick + "!" + user + "@" + server + " PART " + chan + " :" + reason + "\r\n";
    else
        buf = ":" + nick + "!" + user + "@" + server + " PART " + chan + "\r\n";
    return (buf); 
}

std::string RPL_INVITING(const std::string & server, const std::string & nick, const std::string & target, const std::string & chan)
{
    std::string buf = ":" + server + " 341 " + nick + " " + target + " " + chan + "\r\n";
    return (buf);
}

std::string RPL_MY_INVITE(const std::string & server, Client & client, const std::string & target, const std::string & chan)
{
    std::string buf = ":" + client.getClientNickname() + "!" + client.getClientUsername() + "@" + server + " INVITE " + target + " " + chan + "\r\n";
    return (buf);   
}

/****************************************************************************/
/*                              ERR Numerics                                */
/****************************************************************************/


std::string ERR_NONICKNAMEGIVEN(const std::string & server, const std::string & nick, const std::string & user)
{
    std::string buf;
    if (nick.empty() && user.empty())
        buf = RED ":" + server + " 431 " + "*" + " :No nickname given" END + "\r\n";
    else if (user.empty())
        buf = RED ":" + server + " 431 " + nick + " :No nickname given" END + "\r\n";
    else if (nick.empty())
        buf = RED ":" + server + " 431 " + user + " :No nickname given" END + "\r\n";
    else
        buf = RED ":" + server + " 431 " + nick + " :No nickname given" END + "\r\n";
    return (buf);
}

std::string ERR_ERRONEUSNICKNAME(const std::string & server, const std::string & nick, const std::string & user)
{
    std::string buf;
    if (user.empty())
        buf = RED ":" + server + " 432 " + "* " + nick + " :Erroneus nickname" END + "\r\n";
    else
        buf = RED ":" + server + " 432 " + user + " " + nick + " :Erroneus nickname" END + "\r\n";
    return (buf);
}

std::string ERR_NICKNAMEINUSE(const std::string & server, const std::string & nick, const std::string & user)
{
    std::string buf;
    if (user.empty())
        buf = RED ":" + server + " 433 " + "* " + nick + " :Nickname is already in use" END + "\r\n";
    else
        buf = RED ":" + server + " 433 " + user + " " + nick + " :Nickname is already in use" END + "\r\n";
    return (buf);
}

std::string ERR_NEEDMOREPARAMS(const std::string & server, const std::string & nick, const std::string & command)
{
    std::string buf;
    if (nick.empty())
        buf = RED ":" + server + " 461 " + "* " + command + " :Not enough parameters" END + "\r\n";
    else
        buf = RED ":" + server + " 461 " + nick + " " + command + " :Not enough parameters" END + "\r\n";
    return (buf);
}

std::string ERR_PASSWDMISMATCH(const std::string & server)
{
    std::string buf = RED ":" + server + " 464 " + "*" + " :Password incorrect" END + "\r\n";
    return (buf);
}

std::string ERR_ALREADYREGISTERED(const std::string & server, const std::string & nick)
{
    std::string buf;
    if (nick.empty())
        buf = RED ":" + server + " 462 " + "*" + " :You may not reregister" END + "\r\n";
    else
        buf = RED ":" + server + " 462 " + nick + " :You may not reregister" END + "\r\n";
    return (buf);
}

std::string ERR_UNKNOWNERROR(const std::string & server, const std::string & nick, const std::string cmd, const std::string & info)
{
    std::string buf;
    if (cmd.empty() && nick.empty())
        buf = RED ":" + server + " 400 " + "* " + "* " + ":" + info + END + "\r\n";
    else if (nick.empty())
        buf = RED ":" + server + " 400 " + "* " + cmd + " " + ":" + info + END + "\r\n";
    else if (cmd.empty())
        buf = RED ":" + server + " 400 " + nick + " " + "* " + ":" + info + END + "\r\n";
    return (buf);
}

std::string ERR_UNKNOWNCOMMAND(const std::string & server, const std::string & nick, const std::string & cmd)
{
    std::string buf;
    if (nick.empty())
        buf = RED ":" + server + " 421 " + "* " + cmd + " :Unknown command" END + "\r\n";
    else
        buf = RED ":" + server + " 421 " + nick + " " + cmd + " :Unknown command" END + "\r\n";
    return (buf);
}

std::string ERR_NOORIGIN(const std::string & server, const std::string & nick)
{
    std::string buf;
    if (nick.empty())
        buf = RED ":" + server + " 409 " + "*" + " :No origin specified" END + "\r\n";
    else
        buf = RED ":" + server + " 409 " + nick + " :No origin specified" END + "\r\n";
    return (buf);
}

std::string ERR_NOTREGISTERED(const std::string & server, const std::string & nick)
{
    std::string buf;
    if (nick.empty())
        buf = RED ":" + server + " 451 " + "*" + " :You have not registered" END + "\r\n";
    else
        buf = RED ":" + server + " 451 " + nick + " :You have not registered" END + "\r\n";
    return (buf);
}

std::string ERR_NOSUCHCHANNEL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = RED ":" + server + " 403 " + nick + " " + arg + " :No such channel" END + "\r\n";
    return (buf);
}

std::string ERR_BADCHANMASK(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = RED ":" + server + " 476 " + nick + " " + arg + " :Bad Channel Mask" END + "\r\n";
    return (buf);   
}

std::string ERR_USERONCHANNEL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = RED ":" + server + " 443 " + nick + " " + arg + " :is already on channel" END + "\r\n";
    return (buf);  
}

std::string ERR_INVITEONLYCHAN(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = RED ":" + server + " 473 " + nick + " " + arg + " :Cannot join channel (+i)" END + "\r\n";
    return (buf);  
}

std::string ERR_BADCHANNELKEY(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = RED ":" + server + " 475 " + nick + " " + arg + " :Cannot join channel (+k)" END + "\r\n";
    return (buf); 
}

std::string ERR_NOTONCHANNEL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf;
    
    buf = RED ":" + server + " 442 " + nick + " " + arg + "<client> <channel> :You're not on that channel" END + "\r\n";
    return (buf); 
}

std::string ERR_CHANOPRIVSNEEDED(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf = RED ":" + server + " 482 " + nick + " " + arg + " :You're not channel operator" END + "\r\n";
    return (buf); 
}

std::string ERR_UNKNOWNMODE(const std::string & server, const std::string & nick, const std::string & mode)
{
    std::string buf = RED ":" + server + " 472 " + nick + " " + mode + " :is unknown mode char to me" END + "\r\n";
    return (buf);
}

std::string ERR_USERNOTINCHANNEL(const std::string & server, const std::string & nick, const std::string & args, const std::string & channel)
{
    std::string buf = RED ":" + server + " 441 " + nick + " " + args + " " + channel + " :They aren't on that channel" END + "\r\n";
    return (buf);
}

std::string ERR_NOSUCHNICK(const std::string & server, const std::string & nick, const std::string & target)
{
    std::string buf = RED ":" + server + " 401 " + nick + " " + target + " :No such nick/channel" END + "\r\n";
    return (buf);
}

std::string ERR_INVALIDMODEPARAM(const std::string & server, const std::string & nick, const std::string channel, const std::string & mode, const std::string & args)
{
    std::string buf = ":" + server + " 696 " + nick + " " + channel + " " + mode + " " + args + " :Invalid mode parameters" + "\r\n";
    return (buf);
}

std::string ERR_NORECIPIENT(const std::string & server, const std::string & nick, const std::string & cmd)
{
    std::string buf = ":" + server + " 411 " + nick + " :No recipient given " + cmd + "\r\n";
    return (buf);
}

std::string ERR_NOTEXTTOSEND(const std::string & server, const std::string & nick)
{
    std::string buf = ":" + server + " 412 " + nick + " :No text to send" + "\r\n";
    return (buf);
}

std::string ERR_CANNOTSENDTOCHAN(const std::string & server, const std::string & nick, const std::string & channel)
{
    std::string buf = ":" + server + " 404 " + nick + " " + channel + " :Cannot send to channel" + "\r\n";
    return (buf);
}

std::string ERR_BANNEDFROMCHAN(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf = RED ":" + server + " 474 " + nick + " " + arg + " :Cannot join channel (+b)" END + "\r\n";
    return (buf);
}

std::string ERR_CHANNELISFULL(const std::string & server, const std::string & nick, const std::string & arg)
{
    std::string buf = RED ":" + server + " 471 " + nick + " " + arg + " :Cannot join channel (+l)" END + "\r\n";
    return (buf);    
}

/****************************************************************************/
/*                              OTHERS REPLIES                              */
/****************************************************************************/


std::string MODE_REPLY(const std::string &nick, const std::string & channel, const std::string & mode, const std::string & args)
{
    std::string buf;
    buf = ":" + nick + " MODE " + channel + " " + mode;
    if (!args.empty())
        buf += " " + args;
    buf += "\r\n";
    return (buf);
}

std::string KICK_REPLY(const std::string &nick, const std::string &username, const std::string &victimName, const std::string & channel, const std::string & args)
{
    std::string buf;
    buf = ":" + nick + "!" + username + "@localhost " + "KICK " + channel + " " + victimName;
    if (!args.empty())
        buf += " " + args;
    else
        buf += " kicked"; 
    buf += "\r\n";
    return (buf);
}

std::string PRIVMSG_REPLY(const std::string & nick, const std::string & target, const std::string & message)
{
    std::string buf = ":" + nick + " PRIVMSG " + target + " :" + message + "\r\n";
    return (buf);
}

std::string MODE_ERR_REPLY(const std::string & server, const std::string & nick, const std::string & channel, const std::string & reason)
{
     std::string buf = RED ":" + server  + " " + nick + " " + channel + " :" + reason + END + "\r\n";
    return (buf);   
}