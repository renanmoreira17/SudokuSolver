#include "Region.hpp"

template<>
const Tile& Region<Tile>::getTileFromT(const Tile& element) const
{
    return element;
}