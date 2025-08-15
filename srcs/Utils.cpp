#include "Utils.hpp"

extern sig_atomic_t stop;

void signal_handler(int s) 
{  
    std::cout 
        << BLUE 
        << std::endl << std::endl
        << "Signal: " << s << ": Good Bye and see you next time! ;)"
        << std::endl
        << " ____  ____   ___" << std::endl
	    << "(_  _)(  _ \\ / __)" << std::endl
	    << " _)(_  )   /( (__ " << std::endl
	    << "(____)(_)\\_) \\___)" << std::endl
        << std::endl
        << "© 2025 Ircserv, Inc. aciezadl - jaung - ade-fran"
        << END << std::endl << std::endl;
    stop = 1;
    return ;
}

size_t  stringToSizeT(std::string toCast)
{
    if (toCast.find_first_not_of("0123456789") != std::string::npos || toCast.empty())
        return (0);
    errno = 0;
    size_t value = strtoul(toCast.c_str(), NULL, 10);
    if (errno == ERANGE)
        return (0);
    return value;
}