#include "Utils.hpp"

//return nb of spaces " " in data
size_t countSpaces(std::string data) {
    int count = 0;
    for(size_t i = 0; i < data.size(); i++)
        if(data[i] == ' ')
            count ++;
    return count;
}

//return nb of spaces " " in data, between begin and ':', return 0 if ':' not found
size_t countSpacesUntilColon(std::string data) {
    int count = 0;
    size_t pos = data.find(':');
    if(pos == data.size())
        return 0;
    for(size_t i = 0; i < pos; i++)
        if(data[i] == ' ')
            count ++;
    return count;
}