# include "Server.hpp"
# include "Client.hpp"
# include "Utils.hpp"
# include <vector>

int parseReq(int socket_fd, char *buf, Server ircserver) {
    std::string data(buf);
    std::string message;
    Client client;

    if (!data.empty() && data[data.size() - 1] == '\r')
        data.erase(data.size() - 1);
    if (data.rfind("PASS ", 0) == 0) {
        if(countSpaces(data) != 1) {
            std::cout << socket_fd << "<command> :Not enough parameters" << std::endl; //err 461
            //ou err 462 direct ?, a voir comment ca fonctionne
            return 1;
        }
        if(ircserver.getClientsDB().find(socket_fd) != ircserver.getClientsDB().end()) {
            std::cout << socket_fd << " :You may not reregister" << std::endl; //err 462
            return 1;
        }
    }
    if (data.rfind("NICK ", 0) == 0) {
        std::string nickname = data.substr(5);
        if(nickname.empty() || nickname[0] == '\0' || countSpaces(data) != 1) { //j'arrive pas tester avec telnet a voir plus tard
            std::cout << RED << socket_fd << " " << nickname << " :No nickname given" END << std::endl; //err 431
            return 1;
        }
        if(client.checkNicknameForm(nickname) == 1) {
            std::cout << RED << socket_fd << " " << nickname << " :Erroneus nickname" END << std::endl; //err 432
            return 1; 
        }
        if(client.checkNicknameExist(nickname, ircserver) == 1) {
            std::cout << RED << socket_fd << " " << nickname << " :Nickname is already in use" END << std::endl; //err 433
            return 1;
        }
        ircserver.getClientsDB().find(socket_fd)->second.setClientNickname("nickname");
    }
    if (data.rfind("USER ", 0) == 0) {
        if(ircserver.getClientsDB().find(socket_fd) != ircserver.getClientsDB().end()) {
            std::cout << socket_fd << " :You may not reregister" << std::endl; //err 462
            return 1;
        }
    }
    if (data.rfind("JOIN ", 0) == 0) {

    }
    if (data.rfind("PART ", 0) == 0) {
        
    }
    if (data.rfind("TOPIC ", 0) == 0) {

    }
    if (data.rfind("INVITE ", 0) == 0) {
        
    }
    if (data.rfind("KICK ", 0) == 0) {

    }
    //MODE only channel mode (i t k o l)
    if (data.rfind("MODE ", 0) == 0) {
        
    }
    if (data.rfind("PRIVMSG ", 0) == 0) {
        if(data.find(':') == data.size() || countSpacesUntilColon(data) > 2) {
            std::cout << socket_fd << " PRIVMSG :Not enough parameters" << std::endl; //err 461
            return 1;
        }
        if(countSpacesUntilColon(data) == 1) {
            std::cout << socket_fd << " :No recipient given (PRIVMSG)" << std::endl; //err 411
            return 1;
        }
        if(data.find(':') == data.size()-1) {
            std::cout << socket_fd << " :No text to send" << std::endl; //err 412
            return 1;
        }
        std::string nick = data.substr(data.find(' '));
        if(countSpacesUntilColon(data) == 2 && client.checkNicknameExist(nick, ircserver) == 1) { //ajouter checkServerExist() ?
            std::cout << "ok" << std::endl;
            //send message
            return 0;
        }
    }
    else {
        std::cout << socket_fd  << " " << data.substr(0, data.find(" ")) << " :Unknown command" << std::endl; //err 421
        return 1;
    }
    return 0;
}

int receiveReq(int socket_fd, Server ircserver) {
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    ssize_t bytes = recv(socket_fd, buf, 1024, 0);
    if(bytes <= 0) {
        std::cout << RED "Client " << socket_fd << " disconnected" END << std::endl;
        //clear client
        close(socket_fd);
        return 1;
    }
    else {
        buf[bytes] = '\0';
        if(parseReq(socket_fd, buf, ircserver) == 1)
            return 1;
        else
            std::cout << "Parsing req error" << std::endl;
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
                    if (socket_fd == ircserver.getSocket())
                        ircserver.addClient(socket_fd);
                    else
                    {
                        if(receiveReq(socket_fd, ircserver) == 1)
                            std::cout << "error" << std::endl;//send error

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