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
        void                setName(std::string const & name);

        std::string const & getPassword() const;
        std::string const & getTopic() const;
        size_t const &      getLimit() const;
        std::string const & getName() const;
        bool const &        isInviteOnly() const;
        bool const &        topicIsSet() const;
        bool const &        passwordIsSet() const;
        bool const &        getLimitMode() const;
        std::map<std::string, Client> const & getMembers() const;

        void                broadcast(std::string const & message);
        void                addMember(Client & client, std::string const & name);

    private:
        std::map<std::string, Client>   _members;
        std::vector<std::string>        _operators;
        std::string                     _password;
        std::string                     _topic;
        size_t                          _limit;
        std::string                     _name;
        
        bool                            _inviteOnlyIsSet;
        bool                            _topicRestrictionIsSet;
        bool                            _passwordIsSet;
        bool                            _limitIsSet;
};

#endif