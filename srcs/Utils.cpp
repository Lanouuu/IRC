#include "Utils.hpp"

extern sig_atomic_t stop;

void signal_handler(int s) 
{
    std::cout << BLUE <<  "Caught signal " << s << END << std::endl;
    stop = 1;
    return ;
}
