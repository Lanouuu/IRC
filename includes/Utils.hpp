#ifndef UTILS_HPP
# define UTILS_HPP

# include "Server.hpp"

volatile sig_atomic_t stop = 0;

size_t          countSpaces(std::string data);
size_t          countSpacesUntilColon(std::string data);
std::string     convertIntToStr(int nb);

void            signal_handler(int s);

#endif