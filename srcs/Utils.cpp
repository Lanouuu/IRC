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
