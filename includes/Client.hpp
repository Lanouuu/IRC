#ifndef CLIENT_HPP
# define CLIENT_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"

# include <cstdlib>
# include <iostream>
# include <sstream>
# include <exception>
# include <arpa/inet.h>
# include <errno.h>

class   Client
{
    private:
    Client(void);
    
    std::string _clientNickname; //unique in server
    
    int    _clientFd;
    std::string _clientRealname;
    std::string _clientHostAdrr;
    std::string _clientUsername;
    std::string _serverName;
    
    bool _isOperator; //Channel moderators are identified by the channel member prefix '@'
    bool _canKick;
    bool _canMode;
    bool _canInvite;
    bool _canTopic;
    bool _canOperator;
    bool _canLimit;

    public:

        // Client(int ac, char **av);
        ~Client(void);

        std::string getClientNickname();

        int getClientFd();
        std::string getClientRealname();
        std::string getClientHostAddr();
        std::string getClientUsername();
        std::string getClientServerName();

        // void setInviteOnly(); // MODE i
        // void setTopic();      // MODE t
        // void setPassword();   // MODE k
        // void setOperator();   // MODE o
        // void setLimit();      // MODE l
};

#endif