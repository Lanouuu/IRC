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

Client::~Client() 
{
}

/****************************************************************************/
/*                               Operators                                  */
/****************************************************************************/

/****************************************************************************/
/*                           Getters / Setters                              */
/****************************************************************************/

std::string Client::getClientNickname() const {
    return _clientNickname;
}

int Client::getSocket() {
    return _clientSocket;
}

std::string Client::getClientRealname() {
    return _clientRealname;
}

std::string Client::getClientUsername() {
    return _clientUsername;
}

void    Client::setSocket(int socket)
{
    this->_clientSocket = socket;
    return ;
}

void Client::setClientNickname(std::string nick) {
    _clientNickname = nick;
}

/****************************************************************************/
/*                           Members Functions                              */
/****************************************************************************/

//return 1 if exist
int Client::checkNicknameExist(std::string nickName, Server &ircserver) {
    for(std::map<int, Client>::const_iterator it = ircserver.getClientsDB().begin(); it != ircserver.getClientsDB().end(); it++)
    {
        if(it->second.getClientNickname() == nickName)
            return 1;
    }
    return 0;
}

int Client::checkNicknameForm(std::string nickName) {
    if(nickName.find(' ') != std::string::npos || nickName.find(',') != std::string::npos ||
    nickName.find('*') != std::string::npos || nickName.find('?') != std::string::npos ||
    nickName.find('!') != std::string::npos || nickName.find('@') != std::string::npos || 
    nickName.find('.') != std::string::npos || nickName[0] == '$' || nickName[0] == ':' || 
    nickName[0] == '#' || nickName[0] == '&' || nickName[0] == '~' || nickName[0] == ':' ||
    nickName[0] == '%' || nickName[0] == '+')
        return 1;
    return 0;
}

int Client::parseClient(std::string &data, int client_fd, Server &ircserver) {
    std::istringstream iss(data);
    std::string line, password, nickName, realName, userName;

    // ircserver.getClientsDB().insert(std::make_pair<int, Client>(99, Client("alan"))); //test doublon

    while (std::getline(iss, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if(line.rfind("PASS ", 0) == 0)
        {
            password = line.substr(5);
            if(password != ircserver.getServerPassword()) {
                std::cout << RED "Wrong server password : " << password << END << std::endl;
                return 1;
            }
        }
        if(line.rfind("NICK ", 0) == 0)
        {
            nickName = line.substr(5);
            if(nickName.empty() || nickName[0] == '\0') { //j'arrive pas tester avec telnet a voir plus tard
                std::cout << RED << client_fd << " " << nickName << " :No nickname given" END << std::endl; //err 431
                return 1;
            }
            if(checkNicknameForm(nickName) == 1) {
                std::cout << RED << client_fd << " " << nickName << " :Erroneus nickname" END << std::endl; //err 432
                return 1;
            }
            if(checkNicknameExist(nickName, ircserver) == 1) {
                std::cout << RED << client_fd << " " << nickName << " :Nickname is already in use" END << std::endl; //err 433
                return 1;
            }
            _clientNickname = nickName;
        }
        if(line.rfind("USER ", 0) == 0)
        {
            int count = 0;
            size_t pos2 = line.find_last_of(':');
            for(size_t i = 0; i < pos2; i++) {
                if(line[i] == ' ')
                    count ++;
            }
            if(count != 4) {
                std::cout << RED << client_fd << "USER :Not enough parameters" END << std::endl; //err 461
                return 1;
            }
            size_t pos = line.find(' ', 5); // 2eme space pour recup l'username
            userName = line.substr(5, pos - 5);
            if(userName.size() == 0 || userName[0] == 0) {
                std::cout << RED << client_fd << "USER :Not enough parameters" END << std::endl; //err 461
                return 1;
            }
            realName = line.substr(pos2 + 1);
            _clientUsername = userName;
            _clientRealname = realName;
        }
    }
    return 0;
}
