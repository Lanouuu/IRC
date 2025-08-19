#ifndef BOT_HPP
# define BOT_HPP

#include <iostream>
#include <vector>

class   Bot
{
    public:
        
        Bot(void);
        Bot(const Bot & src);
        ~Bot(void);

        Bot &   operator=(const Bot & rhs);

    private:
        std::string _nick;
        std::vector<std::string> _jokes;        

};

#endif