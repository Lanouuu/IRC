#include <Channel.hpp>

Channel::Channel(): _limit(0), _inviteOnlyIsSet(false), _topicRestrictionIsSet(false), _passwordIsSet(false), _limitIsSet(false)
{

}

Channel::~Channel()
{

}

void    Channel::setPassword(std::string const & password)
{
    _password = password;
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

bool const & Channel::isInviteOnly() const
{
    return _inviteOnlyIsSet;
}

bool const &        Channel::getTopicMode() const
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

void    Channel::broadcast(std::string const & message)
{
    for (std::map<std::string, Client>::iterator it = _members.begin(); it != _members.end(); it++)
    {
        it->second.getBufOUT() = message;
    }
}

void    Channel::addMember(Client & client)
{
    _members.insert(std::pair<std::string, Client>(client.getClientNickname(), client));
}