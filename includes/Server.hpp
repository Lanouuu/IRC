#ifndef SERVER_HPP
# define SERVER_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"

# define MAX_EVENTS 1024

# include <cstdlib>
# include <cstring>
# include <iostream>
# include <sstream>
# include <exception>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <errno.h>

class   Server
{
    public:

        
        Server(int ac, char **av, int epoll_fd);
        ~Server(void);

        uint16_t    getPort(void) const;

    private:
    
        Server(void);

        std::string _serverIP;
        uint16_t    _serverPort;
        std::string _serverPassword;
        sockaddr_in _serverStruct;
        int         _serverSocket;

        void        checkArgs(int ac);
        void        parsePort(std::string & port);
        void        parsePassWord(std::string & password);
        void        passwordErr(char c);
        void        fillStruct(void);
        void        fillSocket(void);
        void        launchServer(int epoll_fd);

};
  
#endif