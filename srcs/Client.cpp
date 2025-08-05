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

Client::Client(const std::string &nickname){
    _isOperator=false; 
    _canKick=false;
    _canMode=false;
    _canInvite=false;
    _canTopic=false;
    _canOperator=false;
    _canLimit=false;
    _clientNickname = nickname;
}

Client::~Client() {

}

std::string Client::getClientNickname() const {
    return _clientNickname;
}

int Client::getClientFd() const {
    return _clientFd;
}

std::string Client::getClientRealname() const {
    return _clientRealname;
}

// std::string Client::getClientHostAddr() const {
//     return _clientHostAdrr;
// }

std::string Client::getClientUsername() const {
    return _clientUsername;
}

// std::string Client::getClientServerName() const {
//     return _serverName;
// }
