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
        if((n_event = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1)
            throw std::runtime_error( RED "Error: epoll_wait: " END + std::string(strerror(errno)));
        std::cout << "Event catch" << std::endl;
        for (int i = 0; i < n_event; i++) 
        {
            int socket_fd = events[i].data.fd; // -> correspond a la socket du server;
            if (events[i].events & EPOLLIN) 
            {
                if (socket_fd == _serverSocket)
                    addClient(socket_fd);
                // else
                // {
                //     if(parseReq(socket_fd, buf_client, temp_fd, ircserver) == 1)
                //     //send error
                // }
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

void    Server::addClient(int socket_fd)
{
    Client client_temp;
    int client_fd = accept(socket_fd, NULL, NULL);
    client_temp.setSocket(client_fd);
    client_temp.setServName(_serverName);
    client_temp.setNetwork(_serverNetwork);
    char buf_client[1024];
    recv(client_fd, buf_client, 1024, 0);
    std::string data(buf_client);
    client_temp.parseClient(data, client_fd, *this);
    connectionReply(client_fd, client_temp.getClientNickname());
    _clientsDB.insert(std::make_pair(client_fd, client_temp));
    return ;
}