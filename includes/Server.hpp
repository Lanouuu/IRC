#ifndef SERVER_HPP
# define SERVER_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"


# include <cstdlib>
# include <iostream>
# include <sstream>
# include <exception>
# include <arpa/inet.h>
# include <errno.h>

class   Server
{
    public:

        Server(int ac, char **av);
        ~Server(void);

        uint16_t    getPort(void) const;

    private:
    
        Server(void);

        uint16_t    _serverPort;
        std::string _serverPassword;

        void        checkArgs(int ac);
        void        parsePort(std::string & port);
        void        parsePassWord(std::string & password);
        void        passwordErr(char c);

};
  
#endif