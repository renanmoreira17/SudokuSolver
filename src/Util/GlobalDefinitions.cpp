#include "GlobalDefinitions.hpp"

bool Coordinates::operator==(const Coordinates& other) const
{
    return row == other.row && col == other.col;
}
