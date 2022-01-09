#ifndef __UTILFUNCTIONS_H__
#define __UTILFUNCTIONS_H__

#include "GlobalDefinitions.hpp"

#include <iterator>
#include <sstream>
#include <string>

char convertRowToLetter(TileValueType row);

template<typename ContainerType>
std::string joinContainer(const ContainerType& container, const std::string& separator = ", ")
{
    std::stringstream ss;
    for (auto it = container.cbegin(); it != container.cend(); ++it)
    {
        ss << *it;
        if (std::next(it) != container.cend())
        {
            ss << separator;
        }
    }
    return ss.str();
}

#endif // __UTILFUNCTIONS_H__