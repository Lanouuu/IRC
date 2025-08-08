#include "Client.hpp"

/****************************************************************************/
/*                      Constructors / Destructors                          */
/****************************************************************************/

Client::Client() : _clientSocket(-1)
{
    _isOperator=false; 
    _canKick=false;
    _canMode=false;
    _canInvite=false;
    _canTopic=false;
    _canOperator=false;
    _canLimit=false;
}

Client::Client(const Client & src)
{
    *this = src;
    return ;
}

Client::~Client() 
{
    
}

/****************************************************************************/
/*                               Operators                                  */
/****************************************************************************/

Client &    Client::operator=(const Client & rhs)
{
    this->_clientNickname = rhs._clientNickname;
    this->_clientSocket = rhs._clientSocket;
    this->_clientRealname = rhs._clientRealname;
    this->_clientUsername = rhs._clientUsername;
    this->_serverName = rhs._serverName;
    this->_serverNetwork = rhs._serverNetwork;
    this->_isOperator = rhs._isOperator;
    this->_canKick = rhs._canKick;
    this->_canMode = rhs._canMode;
    this->_canInvite = rhs._canInvite;
    this->_canTopic = rhs._canTopic;
    this->_canOperator = rhs._canOperator;
    this->_canLimit = rhs._canLimit;
    return (*this);
}

/****************************************************************************/
/*                           Getters / Setters                              */
/****************************************************************************/

std::string Client::getClientNickname() const {
    return _clientNickname;
}

int const & Client::getSocket() const {
    return _clientSocket;
}

std::string Client::getClientRealname() {
    return _clientRealname;
}

std::string Client::getClientUsername() {
    return _clientUsername;
}

std::string &   Client::getBufIN(void)
{
    return (_clientBufIN);
}

std::string &   Client::getBufOUT(void)
{
    return (_clientBufOUT);
}

void    Client::setSocket(int socket)
{
    this->_clientSocket = socket;
    return ;
}

void Client::setClientNickname(std::string nick) {
    _clientNickname = nick;
}

void    Client::setServName(std::string & name)
{
    this->_serverName = name;
    return ;
}

void    Client::setNetwork(std::string & network)
{
    this->_serverNetwork = network;
    return ;
}

/****************************************************************************/
/*                           Members Functions                              */
/****************************************************************************/

// return FD of the client to send, ajouter le send pour un channel
int Client::getFDtoSend(std::string nickName, Server &ircserver) {
    for(std::map<int, Client>::const_iterator it = ircserver.getClientsDB().begin(); it != ircserver.getClientsDB().end(); it++)
    {
        if(it->second.getClientNickname() == nickName)
            return it->first;
    }
    return 0;
}