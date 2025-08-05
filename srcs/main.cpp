# include "Server.hpp"
# include "Client.hpp"
# include <vector>
# include <algorithm>

int checkNicknameForm(std::string nickName) {
    if(nickName.find(' ') != std::string::npos || nickName.find(',') != std::string::npos ||
    nickName.find('*') != std::string::npos || nickName.find('?') != std::string::npos ||
    nickName.find('!') != std::string::npos || nickName.find('@') != std::string::npos || 
    nickName.find('.') != std::string::npos || nickName[0] == '$' || nickName[0] == ':' || 
    nickName[0] == '#' || nickName[0] == '&' || nickName[0] == '~' || nickName[0] == ':' ||
    nickName[0] == '%' || nickName[0] == '+')
        return 1;
    return 0;
}

int checkNicknameExist(std::string nickName, Server &ircserver) {
    for(std::map<int, Client>::const_iterator it = ircserver.getClientsDB().begin(); it != ircserver.getClientsDB().end(); it++)
    {
        if(it->second.getClientNickname() == nickName)
            return 1;
    }
    return 0;
}

int parseClient(std::string &data, int temp_fd, Server &ircserver) {
    std::istringstream iss(data);
    std::string line, password, nickName, realName, userName;

    // ircserver.getClientsDB().insert(std::make_pair<int, Client>(99, Client("alan"))); //test doublon

    while (std::getline(iss, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if(line.find("PASS") != std::string::npos)
        {
            password = line.substr(5);
            if(password != ircserver.getServerPassword()) {
                std::cout << RED "Wrong server password : " << password << END << std::endl;
                return 1;
            }
        }
        if(line.find("NICK") != std::string::npos)
        {
            nickName = line.substr(5);
            if(nickName.empty() || nickName[0] == '\0') { //j'arrive pas tester avec telnet a voir plus tard
                std::cout << RED << temp_fd << " " << nickName << " :No nickname given" END << std::endl; //err 431
                return 1;
            }
            std::cout << "line : " << line << std::endl;
            std::cout << "Dans NICK : " << nickName << std::endl;
            if(checkNicknameForm(nickName) == 1) {
                std::cout << RED << temp_fd << " " << nickName << " :Erroneus nickname" END << std::endl; //err 432
                return 1;
            }
            if(checkNicknameExist(nickName, ircserver) == 1) {
                std::cout << RED << temp_fd << " " << nickName << " :Nickname is already in use" END << std::endl; //err 433
                return 1;
            }
        }
        if(line.find("USER") != std::string::npos)
        {
            // commande : USER <username> 0 * <:realname>
            int count = 0;
            size_t pos2 = line.find_last_of(':');
            for(size_t i = 0; i < pos2; i++) {
                if(line[i] == ' ')
                    count ++;
            }
            if(count != 4) {
                std::cout << RED << temp_fd << "USER :Not enough parameters" END << std::endl; //err 461
                return 1;
            }
            size_t pos = line.find(' ', 5); // 2eme space pour recup l'username
            userName = line.substr(5, pos - 5);
            if(userName.size() == 0 || userName[0] == 0) {
                std::cout << RED << temp_fd << "USER :Not enough parameters" END << std::endl; //err 461
                return 1;
            }
            realName = line.substr(pos2 + 1);
        }
    }
    return 0;
}

int parseReq(int socket_fd, char* buf_client, int temp_fd, Server ircserver) {
    (void)socket_fd;
    std::string data(buf_client);
    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line)) {
        if(line.find("CAP LS") != std::string::npos)
            if(parseClient(data, temp_fd, ircserver) == 1)
                return 1;
    }
    return 0;
}

int main(int ac, char **av)
{
    int epoll_fd = -1;
    try
    {
        if ((epoll_fd = epoll_create1(0)) == -1)
            throw std::runtime_error( RED "Error: epoll_create: " END + std::string(strerror(errno)));

        Server              ircserver(ac, av, epoll_fd);
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
                    int temp_fd = accept(socket_fd, NULL, NULL);
                    char buf[1024];
                    char buf_client[1024];
                    memset(&buf, 0, 1024);
                    memset(&buf_client, 0, 1024);
                    recv(socket_fd, buf, 1024, 0);
                    recv(temp_fd, buf_client, 1024, 0);
                    std::cout << "ici" << std::endl;
                    std::cout << buf << std::endl;
                    std::cout << buf_client << std::endl;
                    if(parseReq(socket_fd, buf_client, temp_fd, ircserver) == 1) {
                        //send error
                    }
                    else
                    {
                        //addClient to server map_client
                        (void)temp_fd;
                    }

                }
            }
        }
    }
    catch(const std::exception& e)
    {
        if(epoll_fd != -1)
            close(epoll_fd);
        std::cerr << e.what() << '\n';
        return (1);
    }
    return (0);
}