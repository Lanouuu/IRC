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

bool const &        Channel::getInvitationMode() const
{
    return _inviteOnlyIsSet;
}

bool const &        Channel::getTopicMode() const
{
    return _topicRestrictionIsSet;
}

bool const &        Channel::getPasswordMode() const
{
    return _passwordIsSet;
}

bool const &        Channel::getLimitMode() const
{
    return _limitIsSet;
}

void    Channel::broadcast(std::string const & message) const
{
    for (std::map<std::string, Client>::const_iterator it = _members.begin(); it != _members.end(); it++)
    {
        send(it->second.getSocket(), message.c_str(), message.size(), 0);
    }
}