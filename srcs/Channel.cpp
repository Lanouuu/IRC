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

void    Channel::setPassword(std::string const & password)
{
    _password = password;
    _passwordIsSet = true;
}

void    Channel::setTopic(std::string const & topic)
{
    _topic = topic;
}

void    Channel::setLimit(size_t const & limit)
{
    _limit = limit;
}

void    Channel::setInvitation(std::string const & mode)
{
    if (mode == "y" || mode == "Y" || mode == "YES" || mode == "yes" || mode == "Yes")
        _inviteOnlyIsSet = true;
    else if (mode == "n" || mode == "N" || mode == "NO" || mode == "no" || mode == "No")
        _inviteOnlyIsSet = false;
}

void    Channel::setName(std::string const & name)
{
    _name = name;
}

std::string const & Channel::getPassword() const
{
    return _password;
}

std::string const & Channel::getTopic() const
{
    return _topic;
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

bool const &        Channel::getLimitMode() const
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

bool    Channel::isOperator(const std::string nick) const
{
    std::vector<std::string>::const_iterator it = std::find(_operators.begin(), _operators.end(), nick);
    if(it != _operators.end())
        return true;
    else
        return false;
}

void    Channel::broadcast(std::string const & message, Client & client)
{
    for (std::map<std::string, Client>::iterator it = _members.begin(); it != _members.end(); it++)
    {
        it->second.getBufOUT() =":" + client.getClientNickname() + "!" + client.getClientUsername() + "@localhost JOIN :" + _name + "\r\n";
        send(it->second.getSocket(), it->second.getBufOUT().c_str(), it->second.getBufOUT().size(), 0);
        it->second.getBufOUT().clear();
    }
}

void    Channel::addMember(Client & client)
{
    _members.insert(std::pair<std::string, Client>(client.getClientNickname(), client));
}