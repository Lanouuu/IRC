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
    while (1)
    {
        _tempSet = _masterSet;

        select(_serverFdMax + 1, &_tempSet, NULL, NULL, NULL);
        for (int socket = 0; socket < _serverFdMax; socket++) 
        {
            if (FD_ISSET(socket, &_tempSet)) 
            {
                if (socket == _serverSocket) 
                    addClient(socket);
                else 
                    readClient(socket);
            }
        }
    }
    return ;
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
    return ;
}



/*********Reading Client*********/



void    Server::readClient(int socket_fd)
{
    char    buf[1024];
    size_t  bytes = recv(socket_fd, buf, sizeof(buf), 0);
    if (bytes <= 0) 
    {
        close(socket_fd);
        FD_CLR(socket_fd, &_masterSet);
        std::cout << RED << "Client déconnecté : " << socket_fd << END << std::endl;
    } 
    else 
    {
        bytesReceived(buf, bytes, socket_fd);
    }
    return ;
}

void    Server::bytesReceived(char buf[1024], size_t bytes, int socket_fd)
{
    Client & temp = _clientsDB[socket_fd];

    temp.getBufIN()
}

//print list of clients nicknames in the server
void Server::getClientsList() const {
    std::cout << "List of clients in server " << this->_serverName << " :" << std::endl;
    for(std::map<int, Client>::const_iterator it = this->getClientsDB().begin(); it != this->getClientsDB().end(); it++)
        std::cout << "[" <<it->second.getClientNickname() << "]" << std::endl;
    std::cout << std::endl;
}