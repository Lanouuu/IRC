#ifndef SERVER_HPP
# define SERVER_HPP

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
# include <errno.h>
# include "numerics.hpp"
# include "Client.hpp"
# include "Utils.hpp"
# include "Channel.hpp"
# include "Bot.hpp"

class Client;
class Channel;
class Bot;

typedef std::map<int, Client> client_map;
typedef std::map<std::string, Channel> channel_map;

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
        const channel_map & getChannelDB(void) const;
        channel_map &       getChannelDB(void);
        std::string         getServerName(void) const;
        const Client &      getClient(std::string nick) const;
        Client &            getClient(const std::string &nick);
        
        void                serverListen(void);
        void                clearServer(void);
        void                printClientsList(void) const;
        bool                clientExist(std::string & nick);

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
        channel_map         _channelDB;
        std::string         _serverName;
        std::string         _serverNetwork;
        std::string         _serverVersion;
        std::string         _serverDate;
        Bot                 _bot;

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
        void                readClient(Client & client_temp, int socket_fd);
        void                connectionReply(Client & client_temp);
        void                joinReply(Client & client, Channel const & channel);
        void                checkDisconnectClient(void);
        void                checkEmptyChannel(void);
        void                disconnectToChannel(Client & client);
        void                bytesReceived(Client & client_temp, std::string & buf);
        int                 execCMD(Client & client_temp, std::string & req);
        void                parseCMD(std::string & req, std::string & cmd, std::vector<std::string> & args);

        int                 PASS(Client &  client_temp, std::string & cmd, std::vector<std::string> & args);
        void                NICK(Client &  client_temp, std::vector<std::string> & args);
        void                USER(Client &  client_temp, std::string & cmd, std::vector<std::string> & args);
        void                QUIT(Client & client_temp);
        void                PONG(Client &  client_temp, std::string & cmd, std::vector<std::string> & args);
        void                JOIN(Client & client_temp, std::vector<std::string> & args, std::string const & cmd);
        void                TOPIC(Client &  client_temp, std::vector<std::string> & args);
        void                MODE(Client & client_temp, std::string & cmd, std::vector<std::string> & args);
        void                KICK(Client & client, std::string const & cmd, std::vector<std::string> const & args);
        void                PART(Client &  client_temp, std::vector<std::string> & args);
        void                LIST(Client &  client_temp);
        void                INVITE(Client &  client_temp, std::vector<std::string> & args);
        void                PRIVMSG(Client & client_temp, std::string & cmd, std::vector<std::string> & args);

        bool                ChannelExist(std::string const & name) const;
        bool                hasDuplicates(std::string str, char c, int pos);
        bool                checkModeStr(Client & client_temp, std::string & modeString);
        bool                checkChannel(Client & client_temp, std::string & channelName, std::string & modeString, std::string & cmd);
        bool                execMode(Client & client_temp, Channel & channel, std::string & modeString, std::string & channelName, std::vector<std::string> & args);
        bool                isOnTheChannel(std::string const & name, std::string const & nickname) const;
        void                parseTargets(std::string & targetsStr, std::vector<std::string> & targets);
        void                parseMessage(std::vector<std::string> & args, std::string & message);
        void                sendToChannel(Client & client_temp, std::string & target, std::string & message);
        void                sendToUser(Client & client_temp, std::string & target, std::string & message);         
};
  
#endif