#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"

class Server;

class   Client
{
    public:
        
        Client();
        Client(const Client & src);
        ~Client();

        Client &                operator=(const Client & rhs);

        std::string             getClientNickname() const;
        int                     getFDtoSend(std::string nickName, Server &ircserver);
        int const &             getSocket() const;
        std::string             getClientRealname();
        std::string             getClientUsername();
        std::string &           getBufIN(void);
        std::string &           getBufOUT(void);
        size_t &                getNbCmd(void);
        bool &                  getDisconnectClient(void);
        bool &                  getIsConnected(void);
        bool &                  getIsPass(void);
        void                    setSocket(int socket);
        void                    setClientNickname(std::string nick);
        void                    setClientUserName(std::string & user_name);
        void                    setClientRealName(std::string & real_name);
        void                    setServName(std::string & name);
        void                    setNetwork(std::string & network);
        void                    setDisconnectClient(bool disconnect_client);

    private:

        std::string             _clientNickname;
        int                     _clientSocket;
        std::string             _clientRealname;
        std::string             _clientUsername;
        std::string             _serverName;
        std::string             _serverNetwork;
        std::string             _clientBufIN;
        std::string             _clientBufOUT;
        size_t                  _clientNbCmd;

        bool                    _disconnectClient;
        bool                    _isConnected;
        bool                    _isPass;
        bool                    _isOperator;
        bool                    _canKick;
        bool                    _canMode;
        bool                    _canInvite;
        bool                    _canTopic;
        bool                    _canOperator;
        bool                    _canLimit;
};

#endif