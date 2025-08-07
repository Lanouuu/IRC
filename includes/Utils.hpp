#ifndef UTILS_HPP
# define UTILS_HPP

# include <cstdlib>
# include <arpa/inet.h>
# include <errno.h>
# include "Server.hpp"
# include <signal.h>

size_t          countSpaces(std::string data);
size_t          countSpacesUntilColon(std::string data);
std::string     convertIntToStr(int nb);

void signal_handler(int s);
extern volatile sig_atomic_t stop;

#endif