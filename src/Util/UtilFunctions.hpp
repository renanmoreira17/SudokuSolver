#ifndef __UTILFUNCTIONS_H__
#define __UTILFUNCTIONS_H__

#include "GlobalDefinitions.hpp"

#include <format>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <ostream>

char convertRowToLetter(TileValueType row);

std::string convertRegionSpecificTypeToString(RegionSpecificType regionSpecificType);

namespace detail
{
template<typename T>
void appendToStream(std::ostream& os, const T& value)
{
    os << std::format("{}", value);
}

template<typename T>
void appendToStream(std::ostream& os, const std::shared_ptr<T>& value)
{
    if (value)
    {
        os << std::format("{}", *value);
    }
    else
    {
        os << "<null>";
    }
}
} // namespace detail

template<typename ContainerType>
std::string joinContainer(const ContainerType& container, const std::string& separator = ", ")
{
    std::stringstream ss;
    for (auto it = container.cbegin(); it != container.cend(); ++it)
    {
        detail::appendToStream(ss, *it);
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
        detail::appendToStream(oss, *it);
        ++it;
    }
    while (it != range.end())
    {
        oss << delimiter;
        detail::appendToStream(oss, *it);
        ++it;
    }
    return oss.str();
}

#endif // __UTILFUNCTIONS_H__
