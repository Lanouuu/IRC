#ifndef SERVER_HPP
# define SERVER_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"

# define MAX_EVENTS 1024

#include <ctime>
# include <cstring>
# include <sstream>
# include <exception>
# include <unistd.h>
# include <sys/epoll.h>
# include <map>
# include <utility>
# include "numerics.hpp"
# include "Client.hpp"
#include "Utils.hpp"

class Client;

typedef std::map<int, Client> client_map;

class   Server
{
    public:

        
        Server(int ac, char **av, int epoll_fd);
        ~Server(void);

        uint16_t            getPort(void) const;
        std::string         getServerPassword(void);
        int                 getSocket(void) const;
        client_map &        getClientsDB(void);
        const client_map &  getClientsDB(void) const;

        void                serverListen(int epoll_fd);
        void                addClient(int socket_fd, int epoll_fd);

    private:
    
        Server(void);

        std::string         _serverIP;
        uint16_t            _serverPort;
        std::string         _serverPassword;
        sockaddr_in         _serverStruct;
        int                 _serverSocket;
        client_map          _clientsDB;
        std::string         _serverName;
        std::string         _serverNetwork;
        std::string         _serverVersion;
        std::string         _serverDate;

        void                checkArgs(int ac);
        void                parsePort(std::string & port);
        void                parsePassWord(std::string & password);
        void                passwordErr(char c);
        void                getCreationDate(void);
        void                fillStruct(void);
        void                fillSocket(void);
        void                launchServer(int epoll_fd);
        void                setClient(Client & client, int const & socket_fd, int const & epoll_fd);
        void                connectionReply(int client_fd, const std::string & nick);
        int                 receiveReq(int socket_fd, Server ircserver);
        int                 parseReq(int socket_fd, char *buf, Server ircserver);

};
  
#endif