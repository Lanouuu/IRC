# include "Server.hpp"
# include "Client.hpp"
# include <vector>

// int parseReq(int socket_fd, char* buf_client, int temp_fd, Server ircserver) {
//     (void)socket_fd;
//     std::string data(buf_client);
//     std::istringstream iss(data);
//     std::string line;

//     while (std::getline(iss, line)) {
//         if(line.find("CAP LS") != std::string::npos)
//             if(parseClient(data, temp_fd, ircserver) == 1)
//                 return 1;
//     }
//     return 0;
// }

int main(int ac, char **av)
{
    int epoll_fd = -1;
    try
    {
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
        return (1);
    }
    return (0);
}