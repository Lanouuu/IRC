#ifndef CLIENT_HPP
# define CLIENT_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"

# include <cstdlib>
# include <arpa/inet.h>
# include <errno.h>
# include "Server.hpp"

class Server;

class   Client
{
    private:

        std::string _clientNickname; //unique in server
        int         _clientSocket;
        std::string _clientRealname;
        std::string _clientUsername;
        
        bool        _isOperator; //Channel moderators are identified by the channel member prefix '@'
        bool        _canKick;
        bool        _canMode;
        bool        _canInvite;
        bool        _canTopic;
        bool        _canOperator;
        bool        _canLimit;

        int checkNicknameExist(std::string nickName, Server &ircserver);
        int checkNicknameForm(std::string nickName);
    
    public:

        Client();
        ~Client();

        std::string getClientNickname() const;

        int         getSocket();
        std::string getClientRealname();
        std::string getClientUsername();

        void        setSocket(int socket);
        // void setInviteOnly(); // MODE i
        // void setTopic();      // MODE t
        // void setPassword();   // MODE k
        // void setOperator();   // MODE o
        // void setLimit();      // MODE l

        int parseClient(std::string &data, int client_fd, Server &ircserver);
};

#endif