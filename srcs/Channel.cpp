#include <Channel.hpp>

Channel::Channel(): _limit(0), _inviteOnlyIsSet(false), _topicRestrictionIsSet(false), _passwordIsSet(false), _limitIsSet(false)
{

}


Channel::Channel(Channel const & src)
{
    *this = src;
}

Channel &   Channel::operator=(const Channel & rhs)
{
    _members = rhs._members;
    _operators = rhs._operators;
    _password =  rhs._password;
    _topic = rhs._topic;
    _limit = rhs._limit;
    _name = rhs._name;
    _inviteOnlyIsSet = rhs._inviteOnlyIsSet;
    _topicRestrictionIsSet = rhs._topicRestrictionIsSet;
    _passwordIsSet = rhs._passwordIsSet;
    _limitIsSet = rhs._limitIsSet;
    return *this;
}

Channel::~Channel()
{
}

void    Channel::setPassword(std::string const & mode, std::string const & password)
{
    if (mode == "+")
    {
        _password = password;
        _passwordIsSet = true;
    }
    else
    {
        _password = password;
        _passwordIsSet = false;
    }
}


void    Channel::setSubject(std::string const & subject)
{
    std::string topic = subject;
    if(topic[0] == ':')
        topic.erase(0, 1);
    _subject = topic;
}

void    Channel::setIsTopic(std::string const & mode)
{
    if (mode == "+")
        _topic = true;
    else
        _topic = false;
}

void    Channel::setLimit(std::string const & mode, size_t const & limit)
{
    if (mode == "+")
    {
        std::cout << "CHANNEL SET LIMIT TO " << limit << std::endl;
        _limitIsSet = true;
        _limit = limit;
    }
    else
    {
        _limitIsSet = false;
        _limit = limit;
    }
}

void    Channel::setInvitation(std::string const & mode)
{
    if (mode == "+")
    {
        _inviteOnlyIsSet = true;
        std::cout << "CHANNEL SET TO INVITE ONLY" << std::endl;
    }
    else if (mode == "-")
        _inviteOnlyIsSet = false;
}

void    Channel::setName(std::string const & name)
{
    _name = name;
}

int    Channel::setOperator(Client & client, std::string const & mode, std::string const & name, std::string const & serverName, std::string const & target)
{
    if (mode == "+")
    {
        if (isOperator(name))
            return 1;
        _operators.push_back(name);
    }
    else
    {
        if (_operators.size() == 1 && target == client.getClientNickname())
        {
            //error
            client.getBufOUT() += MODE_ERR_REPLY(serverName, client.getClientNickname(), _name, "Cannot remove operator privileges (you're the last channel operator)");
            return 0;
        }
        else
        {
            std::vector<std::string>::iterator it = std::find(_operators.begin(), _operators.end(), name);
            if (it != _operators.end())
            {
                _operators.erase(it);
                return 1;
            }
            else
            {
                //error
                client.getBufOUT() += MODE_ERR_REPLY(serverName, client.getClientNickname(), _name, "Cannot remove operator privileges, USER " + target + " is not an operator");
                return 0;
            }

        }
    }
    return 1;
}

std::string const & Channel::getPassword() const
{
    return _password;
}

std::string const & Channel::getTopic() const
{
    return _subject;
}

size_t const &      Channel::getLimit() const
{
    return _limit;
}

std::string const & Channel::getName() const
{
    return _name;
}

bool const & Channel::isInviteOnly() const
{
    return _inviteOnlyIsSet;
}

bool const &        Channel::topicIsSet() const
{
    return _topicRestrictionIsSet;
}

bool const &        Channel::passwordIsSet() const
{
    return _passwordIsSet;
}

bool const &        Channel::limitIsSet() const
{
    return _limitIsSet;
}

std::map<std::string, Client> const & Channel::getMembers() const
{
    return _members; 
}

std::map<std::string, Client> & Channel::getMembers()
{
    return _members; 
}

std::vector<std::string> const & Channel::getOperators() const
{
    return _operators;
}

std::map<std::string, std::string> &  Channel::getBanList()
{
    return _banList;
}

std::map<std::string, Client> & Channel::getInviteList()
{
    return _inviteList;
}

bool    Channel::isOperator(const std::string nick) const
{
    std::vector<std::string>::const_iterator it = std::find(_operators.begin(), _operators.end(), nick);
    if(it != _operators.end())
        return true;
    else
        return false;
}

bool    Channel::isOnTheBanList(std::string const & nick, std::string const & realname)
{
    std::cout << RED "BANLIST " << this->getName() << std::endl;
    for (std::map<std::string, std::string>::const_iterator it1 = _banList.begin(); it1 != _banList.end(); it1++)
    {
        std::cout << "[" << it1->first << "] " << "[" << it1->second << "]" << std::endl; 
    }
    std::cout << END << std::endl;
    std::map<std::string, std::string>::const_iterator it = _banList.find(nick);
    if(it != _banList.end())
    {
        if (realname == it->second)
            return true;
    }
    return false;  
}

std::vector<std::string>::iterator Channel::findOperator(std::string const & name)
{
    std::vector<std::string>::iterator it = std::find(_operators.begin(), _operators.end(), name);
    if (it == _operators.end())
        return _operators.end();
    return it;
}

void    Channel::eraseOperator(std::string const & name)
{
    std::vector<std::string>::iterator it = findOperator(name);
    _operators.erase(it);
}

void    Channel::broadcast(std::string const & message)
{
    for (std::map<std::string, Client>::iterator it = _members.begin(); it != _members.end(); it++)
    {
        it->second.getBufOUT() += message;
        if (send(it->second.getSocket(), it->second.getBufOUT().c_str(), it->second.getBufOUT().size(), 0) == -1)
        {
            it->second.getDisconnectClient() = true;
            std::cerr << RED "Error: broadcast send" END << std::endl;
        }
        it->second.getBufOUT().clear();
    }
}

void    Channel::addMember(Client & client)
{
    _members.insert(std::pair<std::string, Client>(client.getClientNickname(), client));
}

void    Channel::eraseMember(Client & client)
{
    _members.erase(client.getClientNickname());
}

void    Channel::addOperator(std::string const & name)
{
    _operators.push_back(name);
}

void    Channel::addInvite(Client const & name)
{
    _inviteList.insert(std::pair<std::string, Client>(name.getClientNickname(), name));
}

void    Channel::eraseInvite(std::string const & name)
{
    std::map<std::string, Client>::iterator it = _inviteList.find(name);
    if(it != _inviteList.end())
        _inviteList.erase(it);
    else
    {
        std::cout << RED "Invite person to delet not found in _inviteList" END << std::endl;
        return ;
    }
}

bool    Channel::isInvite(std::string const & name)
{
    std::map<std::string, Client>::iterator it = _inviteList.find(name);
    if(it != _inviteList.end())
        return true;
    else
        return false;
}

void    Channel::sendToAll(Client & client_temp, std::string & message, int flag)
{
    std ::string buf;
    for (std::map<std::string, Client>::iterator it = _members.begin(); it != _members.end(); it++)
    {
        if (client_temp.getClientNickname() != it->second.getClientNickname())
        {
            if (flag == PRIV_MESSAGE)
                buf = PRIVMSG_REPLY(client_temp.getClientNickname(), this->_name, message);
            else if (flag == QUIT_MESSAGE)
                buf = ":" + client_temp.getClientNickname() + "!" + client_temp.getClientUsername() + "@localhost" + " QUIT :" + message + "\r\n";
            it->second.getBufOUT() += buf;
            if (send(it->second.getSocket(), it->second.getBufOUT().c_str(), it->second.getBufOUT().size(), 0) == -1)
            {
                it->second.getDisconnectClient() = true;
                std::cerr << RED "Error: send: sendToAll() -> " << it->second.getClientNickname() << END << std::endl;
            }
            it->second.getBufOUT().clear();
        }
    }
    return ;
}