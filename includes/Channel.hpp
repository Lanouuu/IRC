#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <map>
#include <string>
#include <vector>

class Client;

class   Channel
{
    public:
        Channel();
        Channel(std::string const i, std::string const t, std::string const k, std::string const l);
        ~Channel();

        void    broadcast() const;
        
    private:
        std::map<std::string, Client>   _members;
        std::vector<std::string>        _operators;
        std::string                     _password;
        std::string                     _topic;
        size_t                          _limit;
        
        bool                            _InviteOnlyIsSet;
        bool                            _topicRestrictionIsSet;
        bool                            _passwordIsSet;
        bool                            _limitIsSet;
};

#endif