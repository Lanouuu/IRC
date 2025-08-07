#include "Server.hpp"

/****************************************************************************/
/*                      Constructors / Destructors                          */
/****************************************************************************/

Server::Server(void)
{
    return ;
}

Server::Server(int ac, char **av, int epoll_fd) 
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
    launchServer(epoll_fd);
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

/****************************************************************************/
/*                           Members Functions                              */
/****************************************************************************/

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

void    Server::launchServer(int epoll_fd)
{
    if (bind(_serverSocket, (struct sockaddr *)&_serverStruct, sizeof(_serverStruct)) == -1)
        throw std::runtime_error(RED "Error: bind: " END + std::string(strerror(errno)));
    if (listen(_serverSocket, 4096) == -1)
        throw std::runtime_error(RED "Error: listen: " END + std::string(strerror(errno)));
    struct epoll_event  event;
    event.events = EPOLLIN;
    event.data.fd = _serverSocket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocket, &event) == -1)
        throw std::runtime_error("Error: epoll_ctl: " + std::string(strerror(errno)));
    return ;
}

void    Server::serverListen(int epoll_fd)
{
    int                 n_event;
    struct epoll_event  events[MAX_EVENTS]; // tableau d'event
    while (1)
    {
        std::cout << "epoll wait" << std::endl;
        if((n_event = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1)
            throw std::runtime_error( RED "Error: epoll_wait: " END + std::string(strerror(errno)));
        std::cout << "Event catch" << std::endl;
        for (int i = 0; i < n_event; i++) 
        {
            int socket_fd = events[i].data.fd; // -> correspond a la socket du server;
            if (events[i].events & EPOLLIN) 
            {
                if (socket_fd == _serverSocket)
                {
                    std::cout << "create client" << std::endl;
                    addClient(socket_fd, epoll_fd);
                }
                else
                {
                    std::cout << "req received" << std::endl;
                    if(receiveReq(socket_fd, *this) == 1)
                    {
                        std::cout << "error in req" << std::endl; 
                    }
                }
            }
        }
    }
    return ;
}

void    Server::connectionReply(int client_fd, const std::string & nick)
{
    std::string welcome = RPL_WELCOME(_serverName, nick, _serverNetwork);
    std::string yourhost = RPL_YOURHOST(_serverName, nick, _serverVersion);
    std::string created = RPL_CREATED(_serverName, nick, _serverDate);
    std::string my_info = RPL_MYINFO(_serverName, nick, _serverVersion);
    std::string isupport = RPL_ISUPPORT(_serverName, nick);
    std::string buf = welcome + yourhost + created + my_info + isupport;

    send(client_fd, buf.c_str(), buf.size(), 0);
    return ;
}

void    Server::setClient(Client & client, int const & socket_fd, int const & epoll_fd)
{
        int clientSocket = accept(socket_fd, NULL, NULL);
        client.setSocket(clientSocket);
        client.getClientEpollStruct().events = EPOLLIN;
        client.getClientEpollStruct().data.fd = clientSocket;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSocket, &client.getClientEpollStruct()) == -1)
            std::cerr << RED << "Error: epoll_ctl: " << strerror(errno) << END << std::endl;
        return ;
}

void    Server::addClient(int socket_fd, int epoll_fd)
{
    Client client_temp;
    
    setClient(client_temp, socket_fd, epoll_fd);
    client_temp.setServName(_serverName);
    client_temp.setNetwork(_serverNetwork);
    char buf_client[1024];
    recv(client_temp.getSocket(), buf_client, 1024, 0);
    std::string data(buf_client);
    client_temp.parseClient(data, client_temp.getSocket(), *this);
    connectionReply(client_temp.getSocket(), client_temp.getClientNickname());
    _clientsDB.insert(std::make_pair(client_temp.getSocket(), client_temp));
    return ;
}

//print list of clients nicknames in the server
void Server::getClientsList() const {
    std::cout << "List of clients in server " << this->_serverName << " :" << std::endl;
    for(std::map<int, Client>::const_iterator it = this->getClientsDB().begin(); it != this->getClientsDB().end(); it++)
        std::cout << "[" <<it->second.getClientNickname() << "]" << std::endl;
    std::cout << std::endl;
}

int Server::parseReq(int socket_fd, char *buf, Server & ircserver) {
    std::string data(buf);
    std::string response;
    Client client;
    if (!data.empty() && data.find('\r') != std::string::npos)
        data.erase(data.find('\r'));
    if (data.find('\n') != std::string::npos)
        data.erase(data.find('\n'));

    getClientsList();

    if (data.rfind("PASS", 0) == 0) {
        if(countSpaces(data) != 1) {
            response = convertIntToStr(socket_fd) + " PASS :Not enough parameters\r\n"; //err 461
            //ou err 462 direct ?, a voir comment ca fonctionne
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        if(_clientsDB.find(socket_fd) != _clientsDB.end()) {

            response = convertIntToStr(socket_fd) + " :You may not reregister\r\n"; //err 462
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        return 0;
    }

    else if (data.rfind("NICK", 0) == 0) {
        std::cout << "in NICK parsing" << std::endl;
        if (data == "NICK") { // a tester avec telnet, 
            std::cout << RED "Nickname already exist" END << std::endl;
            response = convertIntToStr(socket_fd) + " :No nickname is given\r\n"; //err 431
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        std::string nickname = data.substr(5);
        // std::cout << "nickname = [" << nickname << "]" << std::endl;
        if(nickname.empty() || nickname[0] == '\0' || countSpaces(data) != 1) { //j'arrive pas tester avec telnet a voir plus tard
            std::cout << RED "Error no nickname" END << std::endl;
            response = convertIntToStr(socket_fd) + " " + nickname + " :No nickname given\r\n"; //err 431
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        if(client.checkNicknameForm(nickname) == 1) {
            std::cout << RED "Error format nickname" END << std::endl;
            std::map<int, Client>::iterator it = _clientsDB.find(socket_fd);
            if (it != _clientsDB.end()) {
                // response = ":" + this->_serverName + " 432 " + it->second.getClientNickname() + " " + nickname + " :Erroneus nickname\r\n";
                response = convertIntToStr(socket_fd) + " " + nickname + " :Erroneus nickname\r\n"; //err 432
                send(socket_fd, response.c_str(), response.size(), 0);
                return 1; 
            }
            else
                return 1; //send error client not found in clientdb
        }
        if(client.checkNicknameExist(nickname, ircserver) == 1) {
            std::cout << RED "Nickname already exist" END << std::endl;
            response = convertIntToStr(socket_fd) + " " + nickname + " :Nickname is already in use\r\n"; //err 433
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        std::map<int, Client>::iterator it = _clientsDB.find(socket_fd);
        if (it != _clientsDB.end()) {
            std::cout << GREEN "Nickname changed" END << std::endl;
            response = ":" + it->second.getClientNickname() + "!" + "user" + "@" + "host" + " NICK :" + nickname + "\r\n";
            send(socket_fd, response.c_str(), response.size(), 0);
            it->second.setClientNickname(nickname);
            return 0;
        }
        else
            return 1;//send error client not found in clientdb
    }

    else if (data.rfind("USER", 0) == 0) { // a tester avec telnet, automatique à la connexion, Envoyée automatiquement par IRSSI
        if(_clientsDB.find(socket_fd) != _clientsDB.end()) {
            response = convertIntToStr(socket_fd) + " :You may not reregister"; //err 462
            return 1;
        }
        return 0;
    }

    // else if (data.rfind("JOIN", 0) == 0) {

    // }

    // else if (data.rfind("PART", 0) == 0) {
        
    // }

    // else if (data.rfind("TOPIC", 0) == 0) {

    // }

    // else if (data.rfind("INVITE", 0) == 0) {
        
    // }

    // else if (data.rfind("KICK", 0) == 0) {

    // }

    // //MODE only channel mode (i t k o l), et MODE nickname +i (1ere co du client)
    // else if (data.rfind("MODE", 0) == 0) {
        
    // }

    else if (data.rfind("PRIVMSG", 0) == 0) {
        if(data.find(':') == data.size() || countSpacesUntilColon(data) > 2) {
            response = convertIntToStr(socket_fd) + " PRIVMSG :Not enough parameters"; //err 461
            std::cout << RED "Error privmsg not enough paramaters" END << std::endl;
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        if(countSpacesUntilColon(data) == 1) {
            response = convertIntToStr(socket_fd) + " :No recipient given (PRIVMSG)"; //err 411
            std::cout << RED "Error privmsg no recipient" END << std::endl;
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        if(data.find(':') == data.size()-1) {
            response = convertIntToStr(socket_fd) + " :No text to send"; //err 412
            std::cout << RED "Error privmsg no text" END << std::endl;
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        std::string nick_search = data.substr(8, data.find(' ', 8) - 8);
        std::cout << "nick search = " << nick_search << std::endl;
        if(client.checkNicknameExist(nick_search, *this) == 0) {
            response = convertIntToStr(socket_fd) + " :No such nick/channel"; //err 401
            std::cout << RED "Error privmsg no such nick/channel" END << std::endl;
            send(socket_fd, response.c_str(), response.size(), 0);
            return 1;
        }
        if(countSpacesUntilColon(data) == 2 && client.checkNicknameExist(nick_search, ircserver) == 1) { //ajouter checkServerExist() ?
            std::cout << GREEN "SEND msg" END << std::endl;
            // response = ": " & [ this->getClientsDB().find(socket_fd)->first ] + "!~user@host PRIVMSG " + nick_search + data.substr(data.find(':'));
            send(client.getFDtoSend(nick_search, *this), response.c_str(), response.size(), 0);
            return 0;
        }
        std::cout << "return 1" << std::endl;
        return 1;
    }

    else {
        response = convertIntToStr(socket_fd) + " " + data.substr(0, data.find(" ")) + " :Unknown command"; //err 421
        return 1;
    }
}

int Server::receiveReq(int socket_fd, Server ircserver) {
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    ssize_t bytes = recv(socket_fd, buf, 1024, 0);
    std::cout << RED << buf << END << std::endl;
    if(bytes <= 0) {
        std::cout << RED "Client " << socket_fd << " disconnected" END << std::endl;
        //clear client
        close(socket_fd);
        return 1;
    }
    else {
        buf[bytes] = '\0';
        if(parseReq(socket_fd, buf, ircserver) == 1)
        {
            std::cout << "Parsing req error" << std::endl;
            return 1;
        }
        else
            std::cout << "Parsing req ok" << std::endl;
    }
    return 0;
}