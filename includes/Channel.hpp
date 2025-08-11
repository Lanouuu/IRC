#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "Client.hpp"

class Client;

class   Channel
{
    public:
        Channel();
        ~Channel();

        void                setPassword(std::string const & password);
        void                setTopic(std::string const & topic);
        void                setLimit(size_t const & limit);
        void                setInvitation(std::string const & mode);

        std::string const & getPassword() const;
        std::string const & getTopic() const;
        size_t const &      getLimit() const;
        bool const &        isInviteOnly() const;
        bool const &        getTopicMode() const;
        bool const &        passwordIsSet() const;
        bool const &        getLimitMode() const;
        std::map<std::string, Client> const & getMembers() const;
        std::vector<std::string> const & getOperators() const;
        bool                isOperator(const std::string nick) const;

        void                broadcast(std::string const & message);
        void                addMember(Client & client);

    private:
        std::map<std::string, Client>   _members;
        std::vector<std::string>        _operators;
        std::string                     _password;
        std::string                     _topic;
        size_t                          _limit;
        
        bool                            _inviteOnlyIsSet;
        bool                            _topicRestrictionIsSet;
        bool                            _passwordIsSet;
        bool                            _limitIsSet;
};

#endif