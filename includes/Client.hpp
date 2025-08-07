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

        std::string             _clientNickname; //unique in server
        int                     _clientSocket;
        std::string             _clientRealname;
        std::string             _clientUsername;
        std::string             _serverName;
        std::string             _serverNetwork;
        struct epoll_event      _clientEvent;

        bool                    _isOperator; //Channel moderators are identified by the channel member prefix '@'
        bool                    _canKick;
        bool                    _canMode;
        bool                    _canInvite;
        bool                    _canTopic;
        bool                    _canOperator;
        bool                    _canLimit;

        
        public:
        
        Client();
        Client(const Client & src);
        ~Client();

        Client &                operator=(const Client & rhs);

        
        std::string             getClientNickname() const;
        int                     getFDtoSend(std::string nickName, Server &ircserver);
        int                     getSocket();
        std::string             getClientRealname();
        std::string             getClientUsername();
        struct epoll_event &    getClientEpollStruct();
        void                    setSocket(int socket);
        void                    setClientNickname(std::string nick);
        void                    setServName(std::string & name);
        void                    setNetwork(std::string & network);
        
        int                     checkNicknameForm(std::string nickName);
        int                     checkNicknameExist(std::string nickName, Server &ircserver);
        // void setInviteOnly(); // MODE i
        // void setTopic();      // MODE t
        // void setPassword();   // MODE k
        // void setOperator();   // MODE o
        // void setLimit();      // MODE l

        int                     parseClient(std::string &data, int client_fd, Server &ircserver);
};

#endif