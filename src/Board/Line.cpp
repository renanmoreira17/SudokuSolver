#include "Line.hpp"
#include "ElementWrapper.hpp"
#include "ElementList.hpp"
#include "Grid.hpp"
#include "Tile.hpp"
#include <utility>

Line::Line(Grid* grid, Orientation orientation, const TileValueType index)
    : Region(index)
    , m_orientation(orientation)
    , m_grid(grid)
{

    const auto lamb = [&](Tile& tile) {
            if (orientation == vertical)
            {
                tile.setVerticalLine(this);
            }
            else
            {
                tile.setHorizontalLine(this);
            }
    };

    m_elementList = Region<Tile>::make_line(orientation, index, grid->getGridTiles(), std::make_optional(lamb));
}

Line::Line(Line&& other)
: Region(other), m_grid(other.m_grid)
{
    m_index = other.m_index;
    m_orientation = other.m_orientation;

    auto* head = m_elementList->getHead();
    do
    {
        auto& tile = head->getElement();
        if (m_orientation == vertical)
        {
            tile.setVerticalLine(this);
        }
        else
        {
            tile.setHorizontalLine(this);
        }
    } while ((head = head->next()));

    other.m_grid = nullptr;
    other.m_elementList = nullptr;
}

Line& Line::operator=(Line&& other)
{
    if (&other == this) return *this;

    Region::operator=(std::move(other));

    m_grid = other.m_grid;
    m_index = other.m_index;
    m_orientation = other.m_orientation;

    auto* head = m_elementList->getHead();
    do
    {
        auto& tile = head->getElement();
        if (m_orientation == vertical)
        {
            tile.setVerticalLine(this);
        }
        else
        {
            tile.setHorizontalLine(this);
        }
    } while ((head = head->next()));

    other.m_grid = nullptr;
    other.m_elementList = nullptr;

    return *this;
}
