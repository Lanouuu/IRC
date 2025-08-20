#include "Server.hpp"


/****************************************************************************/
/*                      Constructors / Destructors                          */
/****************************************************************************/


Server::Server(void)
{
    return ;
}

Server::Server(int ac, char **av) 
    : _serverIP("0.0.0.0"), 
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

const channel_map    &Server::getChannelDB(void) const
{
    return (this->_channelDB);
}

channel_map    &Server::getChannelDB(void)
{
    return (this->_channelDB);
}

std::string Server::getServerName(void) const
{
    return (this->_serverName);
}

const Client &    Server::getClient(std::string nick) const
{
    std::map<int, Client>::const_iterator it;
    for (it = _clientsDB.begin(); it != _clientsDB.end(); it++)
    {
        if(it->second.getClientNickname() == nick)
            return it->second;
    }
    return it->second;
}

Client &    Server::getClient(const std::string &nick)
{
    std::map<int, Client>::iterator it;
    for (it = _clientsDB.begin(); it != _clientsDB.end(); it++)
    {
        if(it->second.getClientNickname() == nick)
        {
            std::cout << "ici 108" << std::endl;
            return it->second;
        }
    }
    return it->second;
}

/****************************************************************************/
/*                           Members Functions                              */
/****************************************************************************/



/********* Parsing Server Arguments *********/



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



/********* Launching Server *********/



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

        if (select(_serverFdMax + 1, &_tempSet, NULL, NULL, NULL) == -1)
        {
            if (stop)
                return ;
            if (errno == EINTR)
                continue;
            std::cerr << RED "Error: select: " << strerror(errno) << END << std::endl;
            continue ;
        }
        std::cout << BLUE << "Event catch" << END << std::endl;
        for (int socket = 0; socket <= _serverFdMax; socket++) 
        {
            if (FD_ISSET(socket, &_tempSet)) 
            {
                if (socket == _serverSocket) 
                    addClient(socket);
                else
                {
                    Client & client_temp = _clientsDB[socket];
                    readClient(client_temp, socket);
                    connectionReply(client_temp);
                    for (std::map<int, Client>::iterator it = _clientsDB.begin(); it != _clientsDB.end(); ++it) 
                    {
                        Client &    cl = it->second;
                        if (!cl.getBufOUT().empty()) 
                        {
                            std::cout << BLUE << "TO SEND = " << cl.getBufOUT() << END << std::endl;
                            if (send(cl.getSocket(), cl.getBufOUT().c_str(), cl.getBufOUT().size(), 0) == -1)
                            {
                                cl.getDisconnectClient();
                                std::cerr << RED "Error: send" END << std::endl;
                            }
                            cl.getBufOUT().clear();
                        }
                    }
                    checkDisconnectClient();
                    checkEmptyChannel();
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



/********* Adding Client *********/



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
            std::string cap = "CAP END\r\n";
            std::string welcome = RPL_WELCOME(_serverName, client_temp.getClientNickname(), _serverNetwork);
            std::string yourhost = RPL_YOURHOST(_serverName, client_temp.getClientNickname(), _serverVersion);
            std::string created = RPL_CREATED(_serverName, client_temp.getClientNickname(), _serverDate);
            std::string myinfo = RPL_MYINFO(_serverName, client_temp.getClientNickname(), _serverVersion);
            std::string isupport = RPL_ISUPPORT(_serverName, client_temp.getClientNickname());
            client_temp.getBufOUT() += cap + welcome + yourhost + created + myinfo + isupport;
            client_temp.getIsConnected() = true;
        }
    }
    return ;
}



/********* Disconnecting Client *********/



void    Server::disconnectToChannel(Client & client)
{
    for (channel_map::iterator it = _channelDB.begin(); it != _channelDB.end(); it++)
    {
        Channel & chan = it->second;
        if (isOnTheChannel(chan.getName(), client.getClientNickname()))
        {
            std::string message = "Client quit";
            chan.sendToAll(client, message, QUIT_MESSAGE);
            chan.eraseMember(client); 
        }
    }
    return ;
}

void    Server::checkDisconnectClient(void)
{
    for (client_map::iterator it = _clientsDB.begin(); it != _clientsDB.end();)
    {
        
        if (it->second.getDisconnectClient())
        {
            disconnectToChannel(it->second);
            std::cout << BLUE << "Client déconnecté : " << it->second.getSocket() << END << std::endl;
            close(it->second.getSocket());
            FD_CLR(it->second.getSocket(), &_masterSet);
            client_map::iterator temp = it;
            it++;
            _clientsDB.erase(temp);
        }
        else
            it++;
    }
    return ;
}



/********* Reading Client *********/



void    Server::readClient(Client & client_temp, int socket_fd)
{
    char    buf[1024];
    ssize_t bytes;
    bytes = recv(socket_fd, buf, sizeof(buf), 0);
    if (bytes == 0)
        client_temp.getDisconnectClient() = true;
    else if (bytes == -1)
    {
        client_temp.getDisconnectClient() = true;
        client_temp.getBufOUT() += ERR_UNKNOWNERROR(_serverName, client_temp.getClientNickname(), "", "Server Internal Error -> recv()");
    }
    else
    {
        std::string data(buf, bytes);
        bytesReceived(client_temp, data);
    }
    return ;
}

void    Server::bytesReceived(Client & client_temp, std::string & buf)
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



/********* Execute Commands *********/



int    Server::execCMD(Client & client_temp, std::string & req)
{
    std::string                 cmd;
    std::vector<std::string>    args;

    parseCMD(req, cmd, args);
    if (cmd == "CAP")
        return (0) ;
    if (client_temp.getNbCmd() == 0 && cmd != "PASS")
    {
        client_temp.setDisconnectClient(true);
        client_temp.getBufOUT() += ERR_PASSWDMISMATCH(_serverName);
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
    else if (cmd == "QUIT")
        QUIT(client_temp);
    else if (cmd == "JOIN")
        JOIN(client_temp, args, cmd);
    else if (cmd == "PING")
        PONG(client_temp, cmd, args);
    else if (cmd == "TOPIC")
        TOPIC(client_temp, args);
    else if (cmd == "MODE")
        MODE(client_temp, cmd, args);
    else if (cmd == "KICK")
        KICK(client_temp, cmd, args);
    else if (cmd == "PART")
        PART(client_temp, args);
    else if (cmd == "LIST")
        LIST(client_temp);
    else if (cmd == "INVITE")
        INVITE(client_temp, args);
    else if (cmd == "PRIVMSG")
        PRIVMSG(client_temp, cmd, args);
    else
        client_temp.getBufOUT() += ERR_UNKNOWNCOMMAND(_serverName, client_temp.getClientNickname(), cmd);
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



/********* Channels Utils *********/



bool    Server::ChannelExist(std::string const & name) const
{
    if (_channelDB.find(name) == _channelDB.end())
        return false;
    return true;
}

bool    Server::isOnTheChannel(std::string const & name, std::string const & nickname) const
{
    std::map<std::string, Client>::const_iterator it;

    it = _channelDB.at(name).getMembers().find(nickname);
    if (it == _channelDB.at(name).getMembers().end())
        return false;
    return true;
}

void    Server::checkEmptyChannel(void)
{
    for (channel_map::iterator it = _channelDB.begin(); it != _channelDB.end();)
    {
        if (it->second.getMembers().empty())
        {
            channel_map::iterator temp = it;
            it++;
            _channelDB.erase(temp);
        }
        else
            it++;
    }
    return ;
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


/****************************************************************************/
/*                              Commands IRC                                */
/****************************************************************************/


/********* PASS *********/


int    Server::PASS(Client &  client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (client_temp.getIsPass())
    {
        client_temp.getBufOUT() += ERR_ALREADYREGISTERED(_serverName, client_temp.getClientNickname());
        return (0);
    }
    if (args.empty())
    {
        client_temp.getDisconnectClient() = true;
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return (-1);
    }
    if (args[0] != _serverPassword)
    {
        client_temp.getDisconnectClient() = true;
        client_temp.getBufOUT() += ERR_PASSWDMISMATCH(_serverName);
        return (-1);
    }
    client_temp.getIsPass() = true;
    client_temp.getNbCmd()++;
    return (0);
}


/********* NICK *********/


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
        client_temp.getBufOUT() += ERR_NONICKNAMEGIVEN(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername());
        return ;
    }
    if (checkNickFormat(args[0]) == 1)
    {
        client_temp.getBufOUT() += ERR_ERRONEUSNICKNAME(_serverName, args[0], client_temp.getClientUsername());
        return ;
    }
    for (client_map::iterator it = _clientsDB.begin(); it != _clientsDB.end(); it++)
    {
        if (args[0] == it->second.getClientNickname())
        {
            client_temp.getBufOUT() += ERR_NICKNAMEINUSE(_serverName, args[0], client_temp.getClientUsername());
            return ;
        }
    }
    client_temp.setClientNickname(args[0]);
    return ;
}


/********* USER *********/


void    Server::USER(Client &  client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (!client_temp.getClientUsername().empty())
    {
        client_temp.getBufOUT() += ERR_ALREADYREGISTERED(_serverName, client_temp.getClientNickname());
        return ;
    }
    if (args.size() < 4 || args[0].empty() || args[0].find_first_of(" @!") != std::string::npos)
    {
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
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
    if (args[3][0] == ':')
    {
        realname = args[3].substr(1);
        for (size_t i = 4; i < args.size(); i++)
            realname += " " + args[i];
    } 
    else 
        realname = args[3];
    client_temp.setClientUserName(username);
    if (realname.empty())
        realname = "Unknown";
    client_temp.setClientRealName(realname);
    return ;
}


/********* QUIT *********/


void    Server::QUIT(Client & client_temp)
{
    if (!client_temp.getIsConnected())
    {
        client_temp.getBufOUT() += ERR_NOTREGISTERED(_serverName, client_temp.getClientNickname());
        return ;
    }
    client_temp.getDisconnectClient() = true;
    return ;
}


/********* PONG *********/


void    Server::PONG(Client &  client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (args.empty())
    {
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return ;
    }
    if (!client_temp.getIsConnected())
    {
        client_temp.getBufOUT() += ERR_NOTREGISTERED(_serverName, client_temp.getClientNickname());
        return ;
    }
    if (args.empty())
    {
        client_temp.getBufOUT() += ERR_NOORIGIN(_serverName, client_temp.getClientNickname());
        return ;
    }
    client_temp.getBufOUT() += "PONG " + args[0] + "\r\n";
    return ;
}


/********* JOIN *********/


static int  ParseChannelName(std::string const & name)
{
    std::cout << RED << "NAME = " << name << END << std::endl;
    if(name.find_first_of(" ,\a") != std::string::npos)
        return 1;
    return 0;
}

static void createChannel(channel_map & channelDB, std::string const & name, std::string const & password, std::string const & nickname)
{
    channelDB.insert(std::pair<std::string, Channel>(name, Channel()));
    channelDB.at(name).addOperator(nickname);
    if (!password.empty())
        channelDB.at(name).setPassword("+", password);
    channelDB.at(name).setName(name);
}



static void getChannelsAndPassword(std::vector<std::string> const & args,  std::vector<std::pair<std::string, std::string> > & channels)
{
    std::istringstream channelBuf(args[0]);
    std::istringstream passwordBuf;
    std::string currentChannel;
    std::string currentPassword;
    
    if (args.size() > 1)
    passwordBuf.str(args[1]);
    
    while (1)
    {
        if (!std::getline(channelBuf, currentChannel, ','))
        break;
        if (std::getline(passwordBuf, currentPassword, ','))
        channels.push_back(std::pair<std::string, std::string>(currentChannel, currentPassword));
        else
        channels.push_back(std::pair<std::string, std::string>(currentChannel, ""));
    }
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = channels.begin(); it != channels.end(); it++)
    {
        std::cout << RED << "[Channel]-> " << it->first << " [Password]-> " << it->second << END << std::endl;
    }
}

static int parseJoinCommand(Server const & server, Client & client, std::string const & arg)
{
    if (arg[0] != '#')
    {
        client.getBufOUT() += ERR_NOSUCHCHANNEL(server.getServerName(), client.getClientNickname(), arg);
        return 1;
    }
    if (arg.size() == 1)
    {
        client.getBufOUT() += ERR_BADCHANMASK(server.getServerName(), client.getClientNickname(), arg) + client.getClientNickname() + "!" + client.getClientUsername() + "@localhost " + "PART " + arg + " :bye" + "\r\n";
        return 1;
    }
    if (ParseChannelName(arg) == 1)
    {
        std::cout << RED << "ARG = " << arg << END << std::endl;
        client.getBufOUT() += ERR_BADCHANMASK(server.getServerName(), client.getClientNickname(), arg) + client.getClientNickname() + "!" + client.getClientUsername() + "@localhost " + "PART " + arg + " :bye" + "\r\n";
        return 1;
    }
    return 0;
}


void    Server::joinReply(Client & client, Channel const & channel)
{
    std::string topic;
    if (channel.topicIsSet())
        topic = RPL_TOPIC(_serverName, client.getClientNickname(), channel.getName(), channel.getTopic());
    else
        topic = RPL_NOTOPIC(_serverName, client.getClientNickname(), channel.getName());
    std::string namreply = RPL_NAMREPLY(_serverName, client.getClientNickname(), channel);
    std::string endofnames = RPL_ENDOFNAMES(_serverName, client.getClientNickname(), channel.getName());
    client.getBufOUT() += topic + namreply + endofnames;
    return ;
}

static void joinChannel(Client & client, Channel & channel)
{
    std::string message = ":" + client.getClientNickname() + "!" + client.getClientUsername() + "@localhost JOIN :" + channel.getName() + "\r\n";
    channel.addMember(client);
    channel.broadcast(message);
}

static void checkInviteList(Channel & channel, Client & client, std::string const & serverName, std::string const & password)
{
    if (!channel.getInviteList().empty())
    {
        if (!channel.isInvite(client.getClientNickname()))
            client.getBufOUT() += ERR_INVITEONLYCHAN(serverName, client.getClientNickname(), channel.getName());
        else if (channel.passwordIsSet())
        {
            if (password != channel.getPassword())
                client.getBufOUT() += ERR_BADCHANNELKEY(serverName, client.getClientNickname(), channel.getName());
        }
    }
    else
        client.getBufOUT() += ERR_INVITEONLYCHAN(serverName, client.getClientNickname(), channel.getName());
}

void    Server::JOIN(Client & client_temp, std::vector<std::string> & args, std::string const & cmd)
{
    if (args.size() == 0)
    {
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return ;
    }
    std::vector<std::pair<std::string, std::string> > channels;
    getChannelsAndPassword(args, channels);
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = channels.begin(); it != channels.end(); it++)
    {
        if (parseJoinCommand(*this, client_temp, it->first) == 1)
            continue;
        if (!ChannelExist(it->first))
            createChannel(_channelDB, it->first, it->second, client_temp.getClientNickname());
        else    
        {
            if (!_channelDB.at(it->first).isOnTheBanList(client_temp.getClientNickname(), client_temp.getClientRealname()))
            {
                if (_channelDB.at(it->first).limitIsSet() && _channelDB.at(it->first).getLimit() <= _channelDB.at(it->first).getMembers().size())
                     client_temp.getBufOUT() += ERR_CHANNELISFULL(_serverName, client_temp.getClientNickname(), it->first);
                else if (isOnTheChannel(it->first, client_temp.getClientNickname()))
                    client_temp.getBufOUT() += ERR_USERONCHANNEL(_serverName, client_temp.getClientNickname(), it->first);
                else if (_channelDB.at(it->first).isInviteOnly())
                    checkInviteList(_channelDB.at(it->first), client_temp, _serverName, it->second);
                else
                {
                    if (_channelDB.at(it->first).passwordIsSet())
                    {
                        if (args.size() < 2)
                            client_temp.getBufOUT() += ERR_BADCHANNELKEY(_serverName, client_temp.getClientNickname(), it->first);
                        else
                        {
                            if (it->second != _channelDB.at(it->first).getPassword())
                                client_temp.getBufOUT() += ERR_PASSWDMISMATCH(_serverName);
                        }
                    }
                }
            }
            else
                client_temp.getBufOUT() += ERR_BANNEDFROMCHAN(_serverName, client_temp.getClientNickname(), it->first);
        }
        if (client_temp.getBufOUT().empty())
        {
            std::cout << BLUE "CREATING/JOINING CHANNEL " << it->first << " [PASSWORD]-> " << _channelDB.at(it->first).getPassword() << END << std::endl;
            joinChannel(client_temp, _channelDB.at(it->first));
            joinReply(client_temp, _channelDB.at(it->first)); 
        }
    }
}

/********* TOPIC *********/


void Server::TOPIC(Client &  client_temp, std::vector<std::string> & args) {
    std::cout << BLUE "TOPIC COMMAND" END << std::endl;
    if(args.empty())
    {
        std::cout << "ici 696" << std::endl;
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername());
        return;
    }
    std::string channel = args[0];
    if(ChannelExist(channel)) {
        if (channel[0] != '#')
        {
            client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channel);
            return;
        }
        if(!isOnTheChannel(channel, client_temp.getClientNickname()))
        {
            client_temp.getBufOUT() += ERR_NOTONCHANNEL(_serverName, client_temp.getClientNickname(), channel);
            return;
        }
        if(args.size() == 1 && isOnTheChannel(channel, client_temp.getClientNickname()))
        {
            if(!this->_channelDB.at(channel).getTopic().empty())
            {
                std::cout << "ici 713" << std::endl;
                client_temp.getBufOUT() += RPL_TOPIC(_serverName, client_temp.getClientNickname(), channel, this->_channelDB.at(channel).getTopic());
                return;
            }
            else
            {
                std::cout << "ici 721" << std::endl;
                client_temp.getBufOUT() += RPL_NOTOPIC(_serverName, client_temp.getClientNickname(), channel);
                return;
            }
        }
        std::map<std::string, Channel>::iterator it = this->_channelDB.find(channel);
        if(it == _channelDB.end()) {
            client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channel);
            return;
        }
        if(args.size() > 1 && !args[1].empty())
        {
            std::cout << "ici 728" << std::endl;
            //setting topic
            if(it->second.isOperator(client_temp.getClientNickname()) == true)
            {
                std::ostringstream oss;
                for (size_t i = 1; i < args.size(); ++i) {
                    if (i > 1) oss << " ";
                    oss << args[i];
                }
                std::string subject = oss.str();
                it->second.setSubject(subject);
                it->second.broadcast(MY_RPL_TOPIC(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername(), channel, it->second.getTopic()));
                return;
            }
            else
            {
                client_temp.getBufOUT() += ERR_CHANOPRIVSNEEDED(_serverName, client_temp.getClientNickname(), channel);
                return;
            }
        }
        if(args.size() > 1 && args[1] == ":")
        {
            std::cout << "ici 733" << std::endl;
            //unsetting topic
            if(it->second.isOperator(client_temp.getClientNickname()) == true)
            {
                it->second.setSubject("");
                it->second.broadcast(MY_RPL_TOPIC(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername(), channel, it->second.getTopic())); 
            }
            else
                client_temp.getBufOUT() += ERR_CHANOPRIVSNEEDED(_serverName, client_temp.getClientNickname(), channel);
        }
    }
    else
    {
        std::cout << "ici 746" << std::endl;
        client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channel);
    }
}


/********* KICK **********/

void    Server::KICK(Client & client, std::string const & cmd, std::vector<std::string> const & args)
{
    if (args.size() < 2)
    {
        client.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client.getClientNickname(), cmd);
        return ;
    }
    if (ChannelExist(args[0]))
    {
        std::string reason;

        for (size_t i = 2; i < args.size(); i++)
        {
            if (reason.empty())
                reason += args[i];
            else
                reason += " " + args[i];
        }
        if (_channelDB.at(args[0]).isOperator(client.getClientNickname()))
        {
            if (_channelDB.at(args[0]).getMembers().find(args[1]) != _channelDB.at(args[0]).getMembers().end())
            {
                if (args.size() > 2)
                    _channelDB.at(args[0]).broadcast(KICK_REPLY(client.getClientNickname(), client.getClientUsername(), args[1], args[0], reason));
                else
                    _channelDB.at(args[0]).broadcast(KICK_REPLY(client.getClientNickname(), client.getClientUsername(), args[1], args[0], ""));       
                _channelDB.at(args[0]).getBanList().insert(std::pair<std::string, std::string>(args[1], _channelDB.at(args[0]).getMembers().find(args[1])->second.getClientRealname()));
                if (_channelDB.at(args[0]).isOperator(args[1]) && _channelDB.at(args[0]).getOperators().size() == 1)
                {
                    std::cout << args[1] << " is operator" << std::endl;
                    if (_channelDB.at(args[0]).getMembers().size() > 1)
                    {
                        std::cout << "Members left" << std::endl;
                        std::map<std::string, Client>::iterator it2;
                        for(it2 = _channelDB.at(args[0]).getMembers().begin(); it2 != _channelDB.at(args[0]).getMembers().end(); it2++)
                        {
                            if(it2->second.getClientNickname() != client.getClientNickname())
                            {
                                std::cout << "Member found" << std::endl;
                                break;
                            }
                        }
                        std::cout << "Adding to operator list" << std::endl;
                        _channelDB.at(args[0]).addOperator(it2->first);
                        std::string mode = "MODE";
                        std::vector<std::string> arg;
                        arg.push_back(_channelDB.at(args[0]).getName());
                        arg.push_back("+o");
                        arg.push_back(it2->first);
                        MODE(client, mode, arg);
                        // it2->second.getBufOUT() += MODE_REPLY(client.getClientNickname(), _channelDB.at(args[0]).getName(), "+o", it2->second.getClientNickname());
                        // std::cout << it2->second.getBufOUT() << std::endl;
                    }
                    _channelDB.at(args[0]).eraseOperator(args[1]);
                }
                _channelDB.at(args[0]).getMembers().erase(args[1]);
                if (_channelDB.at(args[0]).getMembers().size() == 0)
                    _channelDB.erase(args[0]);
            }
            else
                client.getBufOUT() += ERR_NOTONCHANNEL(_serverName, client.getClientNickname(), cmd);
        }
        else
            client.getBufOUT() += ERR_CHANOPRIVSNEEDED(_serverName, client.getClientNickname(), cmd);
    }
    else
        client.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client.getClientNickname(), cmd);
}

/********* MODE *********/


bool Server::hasDuplicates(std::string str, char c, int pos) 
{
    if (str.find(c, pos + 1) != std::string::npos)
        return true;
    return false;
}

bool Server::checkChannel(Client & client_temp, std::string & channelName, std::string & modeString, std::string & cmd)
{
    if (channelName.empty() || modeString.empty())
    {
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return (false);
    }
    if (channelName[0] != '#')
    {
        client_temp.getBufOUT() += ":" + _serverName + " :MODE for user is not supported" + "\r\n";
        return (false);
    }
    if (!ChannelExist(channelName))
    {
        client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channelName);
        return (false);
    }
    if (!isOnTheChannel(channelName, client_temp.getClientNickname()))
    {
        client_temp.getBufOUT() += ERR_NOTONCHANNEL(_serverName, client_temp.getClientNickname(), channelName);
        return (false);
    }
    return (true);
}

bool Server::checkModeStr(Client & client_temp, std::string & modeString)
{
    if (modeString.find_first_not_of("+-itkol") != std::string::npos)
    {
        std::cout << "ici" << std::endl;
        client_temp.getBufOUT() += ERR_UNKNOWNMODE(_serverName, client_temp.getClientNickname(), modeString);
        return (false);
    }
    if (isalpha(modeString[0]) || !isalpha(modeString[modeString.size() - 1]))
    {
        std::cout << "ici2" << std::endl;
        client_temp.getBufOUT() += ERR_UNKNOWNMODE(_serverName, client_temp.getClientNickname(), modeString);
        return (false);
    }
    for (std::size_t i = 0; i < modeString.size(); i++)
    {
        if (isalpha(modeString[i]))
        {
            if (hasDuplicates(modeString, modeString[i], i))
            {
                client_temp.getBufOUT() += ERR_UNKNOWNMODE(_serverName, client_temp.getClientNickname(), modeString);
                return (false);
            }
        }
        if (modeString[i] == '+' || modeString[i] == '-')
        {
            if (modeString[i + 1] == '+' || modeString[i + 1] == '-')
            {
                client_temp.getBufOUT() += ERR_UNKNOWNMODE(_serverName, client_temp.getClientNickname(), modeString);
                return (false);
            }
        }
    }
    return (true);
}

bool    Server::execMode(Client & client_temp, Channel & channel, std::string & modeString, std::string & channelName, std::vector<std::string> & args)
{
    std::string    actualSign (1, modeString[0]);
    size_t            j = 2;
    for (std::size_t i = 1; i < modeString.size(); i++)
    {
        if (!isalpha(modeString[i]))
            actualSign = modeString[i];
        if (modeString[i] == 'i')
        {
            channel.setInvitation(actualSign);
            channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 'i', ""));
        }
        else if (modeString[i] == 't')
        {
            channel.setIsTopic(actualSign);
            channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 't', ""));
        }
        else if (modeString[i] == 'o')
        {
            if(args.size() > j)
            {
                if (isOnTheChannel(channelName, args[j]))
                {
                    if (channel.setOperator(client_temp, actualSign, args[j], _serverName, args[j]) == 1)
                        channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 'o', args[j]));
                    ++j;
                }
                else
                {
                    client_temp.getBufOUT() += ERR_USERNOTINCHANNEL(_serverName, client_temp.getClientNickname(), args[j], channel.getName());
                    return (false);
                }
            }
            else
            {
                client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), "MODE");
                return (false);
            }
        }
        else if (modeString[i] == 'l')
        {
            if (actualSign == "+")
            {
                if(args.size() > j)
                {
                    size_t len;
                    if ((len = stringToSizeT(args[j])) == 0)
                    {
                        client_temp.getBufOUT() += ERR_INVALIDMODEPARAM(_serverName, client_temp.getClientNickname(), channelName, actualSign + 'l', args[j]);
                        return (false);
                    }
                    channel.setLimit(actualSign, len);
                    channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 'l', args[j]));
                    ++j;
                }
                else
                {
                    client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), "MODE");
                    return (false);
                }
            }
            else
            {
                channel.setLimit(actualSign, 0);
                channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 'l', ""));
            }
        }
        else if (modeString[i] == 'k')
        {
            if (actualSign == "+")
            {
                if (args.size() < j)
                {
                    client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), "MODE");
                    return (false);
                }
                channel.setPassword(actualSign, args[j]);
                channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 'k', args[j]));
                j++;
            }
            else if (actualSign == "-")
            {
                channel.setPassword(actualSign, "");
                channel.broadcast(MODE_REPLY(client_temp.getClientNickname(), channel.getName(), actualSign + 'k', ""));
            }
        }
    }
    return (true);
}

void    Server::MODE(Client & client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (args.size() < 2)
    {
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), cmd);
        return ;
    }
    std::string channelName = args[0];
    std::string modeString = args[1];
    if (!checkChannel(client_temp, channelName, modeString, cmd))
        return ;
    Channel & channel = _channelDB[channelName];
    if (!channel.isOperator(client_temp.getClientNickname()))
    {
        client_temp.getBufOUT() += ERR_CHANOPRIVSNEEDED(_serverName, client_temp.getClientNickname(), channelName);
        return ;
    }
    if (!checkModeStr(client_temp, modeString))
        return ;
    if (!execMode(client_temp, channel, modeString, channelName, args))
           return ;
}


/********* PART *********/


void Server::PART(Client &  client_temp, std::vector<std::string> & args) {
    std::cout << BLUE "PART COMMAND" END << std::endl;
    if(args.empty())
    {
        std::cout << "ici 1073" << std::endl;
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername());
        return;
    }
    std::vector<std::string> channels;
    std::string temp;
    for (size_t i = 0; i < args[0].size(); ++i) {
        if(args[0][i] == ',')
        {
            channels.push_back(temp);
            temp.clear();
            continue ;
        }
        temp += args[0][i];
    }
    if (!temp.empty()) {
    channels.push_back(temp);
    }
    for (size_t i = 0; i < channels.size(); i++)
    {
        if(ChannelExist(channels[i])) {
            if (channels[i][0] != '#')
            {
                client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channels[i]);
                continue ;
            }
            if(!isOnTheChannel(channels[i], client_temp.getClientNickname()))
            {
                client_temp.getBufOUT() += ERR_NOTONCHANNEL(_serverName, client_temp.getClientNickname(), channels[i]);
                continue ;
            }

            std::string reason;
            std::map<std::string, Channel>::iterator it = this->_channelDB.find(channels[i]);
            if(it->second.isOperator(client_temp.getClientNickname()) == true)
            {
                //dernier op et encore du monde dans le chan
                if(it->second.getOperators().size() == 1 && it->second.getMembers().size() > 1)
                {
                    std::map<std::string, Client>::iterator it2;
                    it2 = it->second.getMembers().find(client_temp.getClientNickname());
                    it2++;
                    it->second.addOperator(it2->first);
                    it->second.eraseOperator(client_temp.getClientNickname());
                    it->second.eraseMember(client_temp);
                    if(args.size() > 1)
                    {
                        for(size_t i = 1; i < args.size(); i++) {
                            reason += args[i];
                            if(i + 1 < args.size())
                                reason += " ";
                        }
                    }
                    it->second.broadcast(RPL_PART(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername(), channels[i], reason));
                    it->second.broadcast(":" + _serverName + " MODE " + it->second.getName() + " +o " + it2->first + "\r\n");
                    continue ;
                }
                
                //if last pers on server -> remove server
                if(it->second.getOperators().size() == 1 && it->second.getMembers().size() == 1)
                {
                    std::cout << "ici 1129" << std::endl;
                    _channelDB.erase(it->second.getName());
                }
            }
            else {
                it->second.broadcast(RPL_PART(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername(), channels[i], reason));
                it->second.eraseMember(client_temp);
            }
        }
        else {
            std::cout << "ici 1091" << std::endl;
            client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channels[i]);
        }        
    }
}


/********* LIST *********/


void Server::LIST(Client &  client_temp) {
    std::stringstream ss;
    ss << _serverName + " 321 " + client_temp.getClientNickname() + " Channel :Users Name\r\n";
    std::map<std::string, Channel>::const_iterator it = this->getChannelDB().begin();
    for(; it != this->getChannelDB().end(); it++)
    {
        std::cout << "boucle list" << std::endl;
        std::stringstream ss2;
        ss2 << it->second.getMembers().size();
        std::string member_count = ss2.str();
        if(it->second.getTopic().empty())
            ss <<  _serverName + " 322 " + client_temp.getClientNickname() + " " + it->first + " " + member_count + "\r\n";
        else 
            ss <<  _serverName + " 322 " + client_temp.getClientNickname() + " " + it->first + " " + member_count + " " + ":" + it->second.getTopic() + "\r\n";
    }
    ss << _serverName + " 323 " + client_temp.getClientNickname() +  " :End of LIST\r\n";
    client_temp.getBufOUT() += ss.str();
}


/********* INVITE *********/


bool Server::clientExist(std::string & nick) {
    for (std::map<int, Client>::iterator it = getClientsDB().begin(); it != getClientsDB().end(); ++it) {
        if (it->second.getClientNickname() == nick) {
            return true;
        }
    }
    return false;
}

void Server::INVITE(Client &  client_temp, std::vector<std::string> & args) {
    std::cout << BLUE "INVITE COMMAND" END << std::endl;
    if(args.empty() || args.size() != 2)
    {
        client_temp.getBufOUT() += ERR_NEEDMOREPARAMS(_serverName, client_temp.getClientNickname(), client_temp.getClientUsername());
        return;
    }
    std::string channel = args[1], invited = args[0];
    if(clientExist(invited) == false)
    {
        client_temp.getBufOUT() += ERR_NOSUCHNICK(_serverName, client_temp.getClientNickname(), invited);
        return ;
    }
    if(ChannelExist(channel))
    {
        if(!isOnTheChannel(channel, client_temp.getClientNickname()))
        {
            client_temp.getBufOUT() += ERR_NOTONCHANNEL(_serverName, client_temp.getClientNickname(), channel);
            return ;
        }
        if(this->getChannelDB().find(channel)->second.isInviteOnly())
        {
            if(this->getChannelDB().find(channel)->second.isOperator(client_temp.getClientNickname()))
            {
                if(isOnTheChannel(channel, invited))
                {
                    client_temp.getBufOUT() += ERR_USERONCHANNEL(_serverName, client_temp.getClientNickname(), channel);
                    return ;
                }
                else
                {
                    this->getChannelDB().find(channel)->second.addInvite(getClient(invited));
                    client_temp.getBufOUT() += RPL_INVITING(_serverName, client_temp.getClientNickname(), invited, channel);
                    getClient(invited).getBufOUT() += RPL_MY_INVITE(_serverName, client_temp, invited, channel);
                    return ;
                }
            }
            else
                client_temp.getBufOUT() += ERR_INVITEONLYCHAN(_serverName, client_temp.getClientNickname(), channel);
        }
        else
        {
            if(isOnTheChannel(channel, invited))
            {
                client_temp.getBufOUT() += ERR_USERONCHANNEL(_serverName, client_temp.getClientNickname(), channel);
                return ;
            }
            else
            {
                this->getChannelDB().find(channel)->second.addInvite(getClient(invited));
                client_temp.getBufOUT() += RPL_INVITING(_serverName, client_temp.getClientNickname(), invited, channel);
                getClient(invited).getBufOUT() += RPL_MY_INVITE(_serverName, client_temp, invited, channel);
                return ;
            }
        }
    }
    else {
        client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), channel);
        return ;
    } 
}


/********* PRIVMSG *********/


void    Server::parseTargets(std::string & targetsStr, std::vector<std::string> & targets)
{
    std::istringstream  stream(targetsStr);
    std::string         buf;

    if (targetsStr.find(',') == std::string::npos)
    {
        targets.push_back(targetsStr);
        return ;
    }
    while (std::getline(stream, buf, ','))
        targets.push_back(buf);
    return ;
}

void    Server::parseMessage(std::vector<std::string> & args, std::string & message)
{
    if (args[1][0] == ':') 
    {
        message = args[1].substr(1);
        for (size_t i = 2; i < args.size(); ++i)
            message += " " + args[i];
    } 
    else
        message = args[1];
    return ;
}

void    Server::sendToChannel(Client & client_temp, std::string & target, std::string & message)
{
    if (_channelDB.find(target) == _channelDB.end())
    {
        client_temp.getBufOUT() += ERR_NOSUCHCHANNEL(_serverName, client_temp.getClientNickname(), target);
        return ;
    }
    Channel channel_target = _channelDB[target];
    if (!isOnTheChannel(target, client_temp.getClientNickname()))
    {
        client_temp.getBufOUT() += ERR_CANNOTSENDTOCHAN(_serverName, client_temp.getClientNickname(), target);
        return ;
    }
    channel_target.sendToAll(client_temp, message, PRIV_MESSAGE);
    return ;
}

void    Server::sendToUser(Client & client_temp, std::string & target, std::string & message)
{
    Client  client_target;
    int     flag = 0;
    if (target == "bot")
    {
        _bot.sendJoke(client_temp, message, *this);
        return ;
    }
    for (client_map::iterator it = _clientsDB.begin(); it != _clientsDB.end(); it++)
    {
        if (it->second.getClientNickname() == target)
        {
            client_target = it->second;
            flag = 1;
            break ;
        }
    }
    if (flag == 0)
    {
        client_temp.getBufOUT() += ERR_NOSUCHNICK(_serverName, client_temp.getClientNickname(), target);
        return ;
    }
    std::string buf = PRIVMSG_REPLY(client_temp.getClientNickname(), target, message);
    if (send(client_target.getSocket(), buf.c_str(), buf.size(), 0) == -1)
    {
        std::cerr << RED "Error: send -> sendToUser(): " << target << END << std::endl;
        _clientsDB[client_target.getSocket()].getDisconnectClient() = true;
    }
    return ;
}

void    Server::PRIVMSG(Client & client_temp, std::string & cmd, std::vector<std::string> & args)
{
    if (args.empty())
    {
        client_temp.getBufOUT() += ERR_NORECIPIENT(_serverName, client_temp.getClientNickname(), cmd);
        return ;
    }
    if (args.size() == 1)
    {
        client_temp.getBufOUT() += ERR_NOTEXTTOSEND(_serverName, client_temp.getClientNickname());
        return ;
    }
    std::vector<std::string>    targets;
    std::string                 message;
    parseTargets(args[0], targets);
    parseMessage(args, message);
    if (message.empty())
    {
        client_temp.getBufOUT() += ERR_NOTEXTTOSEND(_serverName, client_temp.getClientNickname());
        return ;
    }
    for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
    {
        if ((*it)[0] == '#')
            sendToChannel(client_temp, *it, message);
        else
            sendToUser(client_temp, *it, message);
    }
    return ;
}
