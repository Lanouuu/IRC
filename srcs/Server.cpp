#include "Server.hpp"


/****************************************************************************/
/*                      Constructors / Destructors                          */
/****************************************************************************/


Server::Server(void)
{
    return ;
}

Server::Server(int ac, char **av) 
    : _serverIP("127.0.0.1"), 
    _serverSocket(-1),
    _serverName("irc.42.fr"), 
    _serverNetwork("42IRC"), 
    _serverVersion("1.0")
{
    checkArgs(ac);
    std::string port = av[1];
    std::string password = av[2];
    parsePort(port);
    parsePassWord(password);
    getCreationDate();
    fillStruct();
    fillSocket();
    launchServer();
    return ;
}

Server::~Server(void)
{
    if (_serverSocket != -1)
        close(_serverSocket);
    // clearDBclients();
    return ;
}


/****************************************************************************/
/*                               Operators                                  */
/****************************************************************************/


/****************************************************************************/
/*                           Getters / Setters                              */
/****************************************************************************/


uint16_t    Server::getPort(void) const
{
    return (this->_serverPort);
}

std::string Server::getServerPassword() {
    return (this->_serverPassword);
}

int Server::getSocket(void) const
{
    return (this->_serverSocket);
}

client_map  &Server::getClientsDB(void)
{
    return (this->_clientsDB);
}

const client_map    &Server::getClientsDB(void) const
{
    return (this->_clientsDB);
}

std::string Server::getServerName(void) const
{
    return (this->_serverName);
}


/****************************************************************************/
/*                           Members Functions                              */
/****************************************************************************/



/*********Parsing Server Arguments*********/



void   Server::checkArgs(int ac)
{
    std::ostringstream buf;

    if (ac < 3)
    {
        buf << RED "Error: " END << "too few arguments"
            << std::endl
            << GREEN "Usage: " END "./ircserv <port> <password>"
            << std::endl;
        throw std::invalid_argument(buf.str());
    }
    if (ac > 3)
    {
        buf << RED "Error: " END << "too many arguments"
            << std::endl
            << GREEN "Usage: " END "./ircserv <port> <password>"
            << std::endl;
        throw std::invalid_argument(buf.str());
    }
    return ;
}

void    Server::parsePort(std::string & port)
{
    long    lport;
    if (port.empty())
        throw std::invalid_argument(RED "Error: " END "Port argument can't be empty");
    if (port.find_first_not_of("0123456789") != std::string::npos)
        throw std::invalid_argument(RED "Error: " END + port + ": invalid Port [0-65535]");
    lport = strtol(port.c_str(), NULL, 10);
    if (errno == ERANGE || lport < 0 || lport > 65535)
        throw std::invalid_argument(RED "Error: " END + port + ": invalid Port [0-65535]");
    _serverPort = lport;
    return ;
}

void    Server::parsePassWord(std::string & password)
{
    if (password.empty())
        throw std::invalid_argument(RED "Error: " END "Password argument can't be empty");
    if (password.length() > 12)
        throw std::invalid_argument(RED "Error: " END "Password must not contain more than 16 characters.");
    for (std::string::size_type i = 0; i < password.length(); i++)
    {
        if (!isalnum(password[i]) && password[i] != '-' && password[i] != '_')
            passwordErr(password[i]);
    }
    _serverPassword = password;
    return ;
}

void    Server::passwordErr(char c)
{
    std::ostringstream buf;

    buf << RED "Error: " END
        << "invalid char \'" << c << "\'"
        << std::endl << std::endl
        << "Password rules: "
        << std::endl
        << "\t- Alphanum char" << std::endl
        << "\t- char \'-\'" << std::endl
        << "\t- char \'_\'" << std::endl;
    throw std::invalid_argument(buf.str());
}



/*********Launching Server*********/



void    Server::getCreationDate(void) 
{
    std::time_t now = std::time(NULL);
    std::tm *gmt = std::gmtime(&now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%a %b %d %Y at %H:%M:%S UTC", gmt);

    _serverDate = buffer;
    return ;
}

void    Server::fillStruct(void)
{
    _serverStruct.sin_family = AF_INET;
    _serverStruct.sin_port = htons(_serverPort);
    if (inet_pton(AF_INET, _serverIP.c_str(), &_serverStruct.sin_addr) != 1)
        throw std::runtime_error(RED "Error: inet_pton: " END + std::string(strerror(errno)));
    return ;
}

void    Server::fillSocket(void)
{
    _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (_serverSocket == -1)
        throw std::runtime_error(RED "Error: socket: " END + std::string(strerror(errno)));
    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error(RED "Error: setsockopt: " END + std::string(strerror(errno)));
    return ;
}

void    Server::launchServer(void)
{
    if (bind(_serverSocket, (struct sockaddr *)&_serverStruct, sizeof(_serverStruct)) == -1)
        throw std::runtime_error(RED "Error: bind: " END + std::string(strerror(errno)));
    if (listen(_serverSocket, 4096) == -1)
        throw std::runtime_error(RED "Error: listen: " END + std::string(strerror(errno)));
    FD_ZERO(&_masterSet);
    FD_ZERO(&_tempSet);
    FD_SET(_serverSocket, &_masterSet);
    _serverFdMax = _serverSocket;
    return ;
}

void    Server::serverListen(void)
{
    while (!stop)
    {
        _tempSet = _masterSet;

        select(_serverFdMax + 1, &_tempSet, NULL, NULL, NULL);
        if (stop)
            return ;
        std::cout << BLUE << "Event catch" << END << std::endl;
        for (int socket = 0; socket <= _serverFdMax; socket++) 
        {
            std::cout << RED " ICI boucle for" END << std::endl;
            if (FD_ISSET(socket, &_tempSet)) 
            {
                std::cout << RED "ICI FD_ISSET" END << std::endl;
                if (socket == _serverSocket) 
                    addClient(socket);
                else
                {
                    Client & client_temp = _clientsDB[socket];
                    readClient(client_temp, socket);
                    connectionReply(client_temp); 
                    if (!client_temp.getBufOUT().empty())
                    {
                        send(socket, client_temp.getBufOUT().c_str(), client_temp.getBufOUT().size(), 0);
                        client_temp.getBufOUT().clear();
                    }
                    checkDisconnectClient(client_temp);
                }
            }
        }
    }
    return ;
}

void    Server::clearServer(void)
{
    for (client_map::iterator it = _clientsDB.begin(); it != _clientsDB.end(); it++)
    {
        close(it->second.getSocket());
        FD_CLR(it->second.getSocket(), &_masterSet);
    }
    close(_serverSocket);
}



/*********Adding Client*********/



int    Server::setClient(Client & client, int const & socket_fd)
{
        int clientSocket;
        if ((clientSocket = accept(socket_fd, NULL, NULL)) == -1)
        {
            std::cerr << RED << "Error: accept: " << strerror(errno) << END << std::endl;
            return (1);
        }
        client.setSocket(clientSocket);
        FD_SET(clientSocket, &_masterSet);
        if (clientSocket > _serverFdMax) 
            _serverFdMax = clientSocket;
        return (0);
}

void    Server::addClient(int socket_fd)
{
    Client client_temp;
    
    if (setClient(client_temp, socket_fd) == 1)
        return ;
    client_temp.setServName(_serverName);
    client_temp.setNetwork(_serverNetwork);
    _clientsDB.insert(std::make_pair(client_temp.getSocket(), client_temp));
    std::cout << BLUE "Client ajoute" END << std::endl;
    return ;
}

void    Server::connectionReply(Client & client_temp)
{
    if (!client_temp.getIsConnected())
    {
        if (client_temp.getIsPass() && !client_temp.getClientNickname().empty() && !client_temp.getClientUsername().empty())
        {
            //std::string cap = "CAP END";
            std::string welcome = RPL_WELCOME(_serverName, client_temp.getClientNickname(), _serverNetwork);
            std::string yourhost = RPL_YOURHOST(_serverName, client_temp.getClientNickname(), _serverVersion);
            std::string created = RPL_CREATED(_serverName, client_temp.getClientNickname(), _serverDate);
            std::string myinfo = RPL_MYINFO(_serverName, client_temp.getClientNickname(), _serverVersion);
            std::string isupport = RPL_ISUPPORT(_serverName, client_temp.getClientNickname());
            client_temp.getBufOUT() = welcome + yourhost + created + myinfo + isupport;
        }
        client_temp.getIsConnected() = true;
    }
    return ;
}

void    Server::checkDisconnectClient(Client & client_temp)
{
    if (client_temp.getDisconnectClient())
    {
        close(client_temp.getSocket());
        FD_CLR(client_temp.getSocket(), &_masterSet);
        _clientsDB.erase(client_temp.getSocket());
    }
    return ;
}



/*********Reading Client*********/



void    Server::readClient(Client & client_temp, int socket_fd)
{
    char    buf[1024];
    if (recv(socket_fd, buf, sizeof(buf), 0) <= 0)
    {
        close(socket_fd);
        FD_CLR(socket_fd, &_masterSet);
        std::cout << RED << "Client déconnecté : " << socket_fd << END << std::endl;
    } 
    else 
        bytesReceived(client_temp, buf);
    return ;
}

void    Server::bytesReceived(Client & client_temp, char buf[1024])
{
   size_t   pos;

   client_temp.getBufIN().append(buf);
   while ((pos = client_temp.getBufIN().find("\r\n")) != std::string::npos)
   {
        std::string req = client_temp.getBufIN().substr(0, pos);
        std::cout << BLUE "REQ = " << req << END << std::endl;
        if (execCMD(client_temp, req) == -1)
            return ;
        client_temp.getBufIN().erase(0, pos + 2);
   }
   return ;
}



/*********Execute Commands*********/



int    Server::execCMD(Client & client_temp, std::string & req)
{
    std::string                 cmd;
    std::vector<std::string>    args;

    parseCMD(req, cmd, args);
    if (cmd == "CAP")
        return (0) ;
    if (client_temp.getNbCmd() == 0 && cmd != "PASS")
    {
        std::cout << RED << "ICI CMD 1 = PASS" END << std::endl;
        client_temp.setDisconnectClient(true);
        client_temp.getBufOUT() = ERR_PASSWDMISMATCH(_serverName);
        return (-1);
    }
    if (cmd == "PASS")
    {
        if (PASS(client_temp, cmd, args) == -1)
            return (-1);
    }
    else if (cmd == "NICK")
        NICK(client_temp, args);
    else if (cmd == "USER")
        USER(client_temp, cmd, args);
    else if (cmd == "JOIN")
        JOIN(client_temp, args);
    return (0);
}

void    Server::parseCMD(std::string & req, std::string & cmd, std::vector<std::string> & args)
{
    std::istringstream  stream(req);
    std::string         buf;
    int                 n_word = 0;

    while (std::getline(stream, buf, ' '))
    {
        if (n_word == 0)
            cmd = buf;
        else
            args.push_back(buf);
        n_word++;
    }
    return ;
}




/*********Commands IRC*********/


bool    Server::ChannelExist(std::string const & name) const
{
    if (_channelDB.find(name) == _channelDB.end())
        return false;
    return true;
}

bool    Server::isAlreadyOnTheChannel(std::string const & name, std::string const & nickname) const
{
    std::map<std::string, Client>::const_iterator it;

    it = _channelDB.at(name).getMembers().find(nickname);
    if (it == _channelDB.at(name).getMembers().end())
        return false;
    return true;
}

int    Server::PASS(Client &  client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (client_temp.getIsPass())
    {
        client_temp.getBufOUT() = ERR_ALREADYREGISTERED(_serverName, client_temp.getClientNickname());
        return (0);
    }
    if (args.empty())
    {
        client_temp.getDisconnectClient() = true;
        client_temp.getBufOUT() = ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return (-1);
    }
    std::cout << RED "size pass = " << args[0].size() << END << std::endl;
    std::cout << RED << args[0] << END << std::endl;
    std::cout << RED << "server pass = " << _serverPassword << END << std::endl;
    if (args[0] != _serverPassword)
    {
        std::cout << RED << "ICI ERR PASS" END << std::endl;
        client_temp.getDisconnectClient() = true;
        client_temp.getBufOUT() = ERR_PASSWDMISMATCH(_serverName);
        return (-1);
    }
    client_temp.getIsPass() = true;
    client_temp.getNbCmd()++;
    return (0);
}

static int checkNickFormat(std::string & nick)
{
    if (nick[0] == '$' || nick[0] == ':' || nick[0] == '#'
        || nick[0] == '~' || nick[0] == '&' || nick[0] == '+' || nick[0] == '%')
        return (1);
    if (nick.find_first_of(" ,*?!@.") != std::string::npos)
        return (1);
    if (nick.size() > 9)
        return (1);
    return (0);
}

void    Server::NICK(Client &  client_temp, std::vector<std::string> & args)
{
    if (args.empty())
    {
        client_temp.getBufOUT() = ERR_NONICKNAMEGIVEN(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername());
        return ;
    }
    if (checkNickFormat(args[0]) == 1)
    {
        client_temp.getBufOUT() = ERR_ERRONEUSNICKNAME(_serverName, args[0], client_temp.getClientUsername());
        return ;
    }
    for (client_map::iterator it = _clientsDB.begin(); it != _clientsDB.end(); it++)
    {
        if (args[0] == it->second.getClientNickname())
        {
            client_temp.getBufOUT() = ERR_NICKNAMEINUSE(_serverName, args[0], client_temp.getClientUsername());
            return ;
        }
    }
    client_temp.setClientNickname(args[0]);
    client_temp.getNbCmd()++;
    return ;
}

void    Server::USER(Client &  client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (!client_temp.getClientUsername().empty())
    {
        client_temp.getBufOUT() = ERR_ALREADYREGISTERED(_serverName, client_temp.getClientNickname());
        return ;
    }
    if (args.size() < 4 || args[0].empty() || args[0].find_first_of(" @!") != std::string::npos)
    {
        client_temp.getBufOUT() = ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return ;
    }
    std::string username;
    if (args[0].size() > 12)
        username = args[0].substr(0, 12);
    else
        username = args[0];
    std::string mode = args[1];
    std::string unused = args[2];
    std::string realname;
    if (args[4][0] == ':')
    {
        realname = args[4].substr(1);
        for (size_t i = 5; i < args.size(); i++)
            realname += " " + args[i];
    } 
    else 
        realname = args[4];
    client_temp.setClientUserName(username);
    if (realname.empty())
        realname = "Unknown";
    client_temp.setClientRealName(realname);
    client_temp.getNbCmd()++;
    return ;
}

static int  ParseChannelName(std::string const & name)
{
    if(name.find_first_of(" ,\a") != std::string::npos)
        return 1;
    return 0;
}

static void createChannel(channel_map & channelDB, Client & client, std::string const & name)
{
    (void)client;
    channelDB.insert(std::pair<std::string, Channel>(name, Channel()));
}


static int parseJoinCommand(Server const & server, Client & client, std::vector<std::string> & args)
{
    if (args.size() == 0)
    {
        client.getBufOUT() = ERR_NEEDMOREPARAMS(server.getServerName(), client.getClientNickname(), args[0]);
        return 1;
    }
    if (args[0][0] != '#')
    {
        client.getBufOUT() = ERR_NOSUCHCHANNEL(server.getServerName(), client.getClientNickname(), args[0]);
        return 1;
    }
    if (ParseChannelName(args[0]) == 1)
    {
        client.getBufOUT() = ERR_BADCHANMASK(server.getServerName(), client.getClientNickname(), args[0]);
        return 1;
    }
    return 0;
}

void    Server::JOIN(Client & client_temp, std::vector<std::string> & args)
{
    std::cout << BLUE "JOIN COMMAND" END << std::endl;
    if (parseJoinCommand(*this, client_temp, args) == 1)
        return ;
    if (ChannelExist(args[0]))
    {
        if (isAlreadyOnTheChannel(args[0], client_temp.getClientNickname()))
            client_temp.getBufOUT() = ERR_USERONCHANNEL(_serverName, client_temp.getClientNickname(), args[0]);
        else if (_channelDB.at(args[0]).isInviteOnly())
            client_temp.getBufOUT() = ERR_INVITEONLYCHAN(_serverName, client_temp.getClientNickname(), args[0]);
        else
        {
            if (_channelDB.at(args[0]).passwordIsSet())
            {
                if (args.size() < 2)
                    client_temp.getBufOUT() = ERR_BADCHANNELKEY(_serverName, client_temp.getClientNickname(), args[0]);
                else
                {
                    if (args[1] == _channelDB.at(args[0]).getPassword())
                        _channelDB.at(args[0]).addMember(client_temp);
                    else
                        client_temp.getBufOUT() = ERR_PASSWDMISMATCH(_serverName);
                }
            }
            else
                _channelDB.at(args[0]).addMember(client_temp);
        }
        return ;
    }
    std::cout << BLUE "CREATING CHANNEL" END << std::endl;
    createChannel(_channelDB, client_temp, args[0]);
    _channelDB.at(args[0]).addMember(client_temp);

}
/*********Print Clients list*********/



void Server::printClientsList(void) const 
{
    std::cout << "List of clients in server " << this->_serverName << " :" << std::endl;
    for(std::map<int, Client>::const_iterator it = this->getClientsDB().begin(); it != this->getClientsDB().end(); it++)
        std::cout << "[" <<it->second.getClientNickname() << "]" << std::endl;
    std::cout << std::endl;
    return ;
}