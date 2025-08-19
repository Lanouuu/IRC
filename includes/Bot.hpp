#ifndef BOT_HPP
# define BOT_HPP

#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

class Server;
class Client;

class   Bot
{
    public:
        
        Bot(void);
        ~Bot(void);

        void    sendJoke(Client & client, std::string const & message, Server & server);

    private:
        std::string _nick;
        std::vector<std::string> _jokes;        

};

#endif