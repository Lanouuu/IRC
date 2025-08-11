#include "Client.hpp"


/****************************************************************************/
/*                      Constructors / Destructors                          */
/****************************************************************************/


Client::Client(void) 
    : _clientSocket(-1),
    _clientNbCmd(0),
    _disconnectClient(false),
    _isConnected(false),
    _isPass(false),
    _isOperator(false), 
    _canKick(false),
    _canMode(false),
    _canInvite(false),
    _canTopic(false),
    _canOperator(false),
    _canLimit(false)
{
   return ;
}

Client::Client(const Client & src)
{
    *this = src;
    return ;
}

Client::~Client(void) 
{
    return ;
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
    this->_clientBufIN = rhs._clientBufIN;
    this->_clientBufOUT = rhs._clientBufOUT;
    this->_clientNbCmd = rhs._clientNbCmd;
    this->_disconnectClient = rhs._disconnectClient;
    this->_isConnected = rhs._isConnected;
    this->_isPass = rhs._isPass;
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
    return (this->_clientNickname);
}

int const & Client::getSocket() const {
    return (this->_clientSocket);
}

std::string Client::getClientRealname() {
    return (this->_clientRealname);
}

std::string Client::getClientUsername() {
    return (this->_clientUsername);
}

std::string &   Client::getBufIN(void)
{
    return (this->_clientBufIN);
}

std::string &   Client::getBufOUT(void)
{
    return (this->_clientBufOUT);
}

size_t &    Client::getNbCmd(void)
{
    return (this->_clientNbCmd);
}

bool &  Client::getDisconnectClient(void)
{
    return (this->_disconnectClient);
}

bool &  Client::getIsConnected(void)
{
    return (this->_isConnected);
}

bool &  Client::getIsPass(void)
{
    return (this->_isPass);
}

void    Client::setSocket(int socket)
{
    this->_clientSocket = socket;
    return ;
}

void    Client::setClientNickname(std::string nick) {
    this->_clientNickname = nick;
}

void    Client::setClientUserName(std::string & user_name)
{
    this->_clientUsername = user_name;
    return ;
}

void    Client::setClientRealName(std::string & real_name)
{
    this->_clientRealname = real_name;
    return ;
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

void    Client::setDisconnectClient(bool disconnect_client)
{
    this->_disconnectClient = disconnect_client;
    return ;
}


/****************************************************************************/
/*                           Members Functions                              */
/****************************************************************************/

