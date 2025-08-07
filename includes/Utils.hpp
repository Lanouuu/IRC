#ifndef UTILS_HPP
# define UTILS_HPP

# include <cstdlib>
# include <arpa/inet.h>
# include <errno.h>
# include "Server.hpp"

size_t          countSpaces(std::string data);
size_t          countSpacesUntilColon(std::string data);
std::string     convertIntToStr(int nb);
#endif