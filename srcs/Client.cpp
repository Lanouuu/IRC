#include "Client.hpp"

//Client(){} ??

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
