# include "Server.hpp"

int main(int ac, char **av)
{
    try
    {
        Server  ircserver(ac, av);
        ircserver.serverListen();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return (1);
    }
    return (0);
}