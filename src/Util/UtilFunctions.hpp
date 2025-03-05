#ifndef __UTILFUNCTIONS_H__
#define __UTILFUNCTIONS_H__

#include "GlobalDefinitions.hpp"

#include <iterator>
#include <sstream>
#include <string>

char convertRowToLetter(TileValueType row);

std::string convertRegionSpecificTypeToString(RegionSpecificType regionSpecificType);

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

Coordinates transformRelativeSubgridToAbsoluteCoordinates(const Coordinates& relativeCoordinates,
                                                          TileValueType subgridIndex);
Coordinates transformSubgridIndexedToAbsoluteCoordinates(TileValueType indexedCoordinate,
                                                         TileValueType subgridIndex);

template<typename Range>
std::string join(const Range& range, const std::string& delimiter)
{
    std::ostringstream oss;
    auto it = range.begin();
    if (it != range.end())
    {
        oss << *it;
        ++it;
    }
    while (it != range.end())
    {
        oss << delimiter << *it;
        ++it;
    }
    return oss.str();
}

#endif // __UTILFUNCTIONS_H__