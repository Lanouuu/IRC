#include "Bot.hpp"
#include "Server.hpp"

Bot::Bot(void): _nick("bot")
{
    _jokes.push_back("HI");
}

Bot::~Bot(void)
{

}

void    Bot::sendJoke(Client & client, std::string const & message, Server & server)
{
    std::string buf;
    if (message == "envoie moi une blague" || message == "send me a joke")
    {
        srand(time(0));
        size_t i = rand() % _jokes.size();
        std::string joke = _jokes[i];
    
        buf = PRIVMSG_REPLY(_nick, client.getClientNickname(), joke);
    }
    else
    {
        buf = PRIVMSG_REPLY(_nick, client.getClientNickname(), "Sorry i didn't understand what you said\r\n");
    }
    if (send(client.getSocket(), buf.c_str(), buf.size(), 0) == -1)
    {
        std::cerr << RED "Error: send -> sendToUser(): " << client.getClientNickname() << END << std::endl;
        server.getClientsDB()[client.getSocket()].getDisconnectClient() = true;
    }
}