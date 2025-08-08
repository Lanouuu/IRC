# include "Server.hpp"

int main(int ac, char **av)
{
    try
    {
        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if(sigaction(SIGINT, &sa, NULL) == -1)
            throw std::runtime_error( RED "Error: sigaction: " END + std::string(strerror(errno)));

        Server  ircserver(ac, av);
        ircserver.serverListen();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        if (stop == 1)
            return (0);
        return (1);
    }
    return (0);
}