#include "Bot.hpp"
#include "Server.hpp"

Bot::Bot(void): _nick("bot")
{
    std::string joke1 = "Pourquoi les mammouths ont-ils disparu ?   Reponse: Parce qu'il n'y avait plus de pappouths ;D\r\n";
    std::string joke2 = "Une petite fille fait de la balancoire et tombe, pourquoi ?   Reponse: Parce qu'elle n'a pas de bras ;D\r\n";
    std::string joke3 = "Pourquoi dit-on que les bretons sont tous freres et soeurs ?   Reponse: Parce qu'ils n'ont Quimper ;D\r\n";
    std::string joke4 = "Que faisaient les dinosaures quand ils n'arrivaient pas a prendre une decision ?   Reponse: Ils faisaient un Tirageosaure ;D\r\n";
    std::string joke5 = "Qu'est-ce qu'un canife ?   Reponse: Un petit fien ;D\r\n";
    std::string joke6 = "Quel est le moyen de transport prefere des dinosaures ?   Reponse: Le velociraptor ;D\r\n";
    std::string joke7 = "Qu'est-ce qui est jaune et qui attend ?   Reponse: C'est JONATHAN !!!!! ;D\r\n";
    _jokes.push_back(joke1);
    _jokes.push_back(joke2);
    _jokes.push_back(joke3);
    _jokes.push_back(joke4);
    _jokes.push_back(joke5);
    _jokes.push_back(joke6);
    _jokes.push_back(joke7);
    return ;
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
        buf = PRIVMSG_REPLY(_nick, client.getClientNickname(), "Sorry i didn't understand what you said\r\n");
    if (send(client.getSocket(), buf.c_str(), buf.size(), 0) == -1)
    {
        std::cerr << RED "Error: send -> sendToUser(): " << client.getClientNickname() << END << std::endl;
        server.getClientsDB()[client.getSocket()].getDisconnectClient() = true;
    }
}