#ifndef SERVER_HPP
# define SERVER_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"

# include <ctime>
# include <cstdlib>
# include <cstring>
# include <sstream>
# include <exception>
# include <unistd.h>
# include <map>
# include <vector>
# include <utility>
# include <arpa/inet.h>
# include <sys/select.h>
# include <signal.h>
# include <errno.h>
# include "numerics.hpp"
# include "Client.hpp"
# include "Utils.hpp"

class Client;

typedef std::map<int, Client> client_map;

class   Server
{
    public:

        Server(int ac, char **av);
        ~Server(void);

        uint16_t            getPort(void) const;
        std::string         getServerPassword(void);
        int                 getSocket(void) const;
        client_map &        getClientsDB(void);
        const client_map &  getClientsDB(void) const;
        void                getClientsList() const;
        std::string         getServerName(void) const;

        void                serverListen(void);

    private:
    
        Server(void);

        fd_set              _masterSet;
        fd_set              _tempSet;
        int                 _serverFdMax;
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
        void                launchServer(void);
        void                addClient(int socket_fd);
        int                 setClient(Client & client, int const & socket_fd);
        void                readClient(int socket_fd);
        void                bytesReceived(char buf[1024], int socket_fd);
        void                execCMD(Client & client_temp, std::string & req);
        void                parseCMD(std::string & req, std::string & cmd, std::vector<std::string> & args);

        void                PASS(Client &  client_temp, std::vector<std::string> & args);
        void                NICK(Client &  client_temp, std::vector<std::string> & args);
        void                USER(Client &  client_temp, std::vector<std::string> & args);
};
  
#endif