# include "Server.hpp"

int main(int ac, char **av)
{
    int epoll_fd = -1;
    try
    {
        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if(sigaction(SIGINT, &sa, NULL) == -1)
            throw std::runtime_error( RED "Error: sigaction: " END + std::string(strerror(errno)));

        if ((epoll_fd = epoll_create1(0)) == -1)
            throw std::runtime_error( RED "Error: epoll_create: " END + std::string(strerror(errno)));
        Server  ircserver(ac, av, epoll_fd);
        ircserver.serverListen(epoll_fd);
    }
    catch(const std::exception& e)
    {
        if(epoll_fd != -1)
            close(epoll_fd);
        std::cerr << e.what() << '\n';
        if (stop == 1)
            return (0);
        return (1);
    }
    return (0);
}