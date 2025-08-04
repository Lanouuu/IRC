# include "Server.hpp"

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
                    (void)temp_fd;
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