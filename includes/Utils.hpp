#ifndef UTILS_HPP
# define UTILS_HPP

# define GREEN 	"\033[0;32m"
# define RED	"\033[0;31m"
# define BLUE	"\033[0;36m"
# define END	"\033[0m"

#include <signal.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <cstdlib>
extern sig_atomic_t stop;

void    signal_handler(int s);
size_t  stringToSizeT(std::string toCast);

#endif