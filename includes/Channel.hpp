#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <map>
#include <string>
#include <vector>
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

        std::map<std::string, Client> const &   getMembers() const;
        std::string const & getPassword() const;
        std::string const & getTopic() const;
        size_t const &      getLimit() const;
        bool const &        getInvitationMode() const;
        bool const &        getTopicMode() const;
        bool const &        getPasswordMode() const;
        bool const &        getLimitMode() const;

        void                broadcast(std::string const & message) const;
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