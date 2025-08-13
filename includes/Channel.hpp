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
        Channel(Channel const & src);
        ~Channel();
        Channel &           operator=(const Channel & rhs);
        void                setPassword(std::string const & mode, std::string const & password);
        void                setSubject(std::string const & subject);
        void                setIsTopic(std::string const & mode);
        void                setLimit(std::string const & mode, size_t const & limit);
        void                setInvitation(std::string const & mode);
        void                setName(std::string const & name);
        void                setOperator(std::string const & mode, std::string const & name);

        std::string const & getPassword() const;
        std::string const & getTopic() const;
        size_t const &      getLimit() const;
        std::string const & getName() const;
        bool const &        isInviteOnly() const;
        bool const &        topicIsSet() const;
        bool const &        passwordIsSet() const;
        bool const &        getLimitMode() const;
        std::map<std::string, Client> const & getMembers() const;
        std::map<std::string, Client> & getMembers();
        std::vector<std::string> const & getOperators() const;
        bool                isOperator(const std::string nick) const;

        void                broadcast(std::string const & message, Client & client);
        void                addMember(Client & client);

    private:
        std::map<std::string, Client>   _members;
        std::vector<std::string>        _operators;
        std::string                     _password;
        std::string                     _subject;
        size_t                          _limit;
        std::string                     _name;
        size_t                          _nbOperator;
        
        bool                            _inviteOnlyIsSet;
        bool                            _topicRestrictionIsSet;
        bool                            _passwordIsSet;
        bool                            _limitIsSet;
        bool                            _topic;
};

#endif