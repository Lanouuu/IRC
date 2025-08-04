#include "Client.hpp"

Client::Client(){
    _isOperator=false; 
    _canKick=false;
    _canMode=false;
    _canInvite=false;
    _canTopic=false;
    _canOperator=false;
    _canLimit=false;
}

Client::~Client() {

}

std::string Client::getClientNickname() {
    return _clientNickname;
}

int Client::getClientFd() {
    return _clientFd;
}

std::string Client::getClientRealname() {
    return _clientRealname;
}

std::string Client::getClientHostAddr() {
    return _clientHostAdrr;
}

std::string Client::getClientUsername() {
    return _clientUsername;
}

std::string Client::getClientServerName() {
    return _serverName;
}
