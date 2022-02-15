#include "Line.hpp"
#include "ElementList.hpp"
#include "ElementWrapper.hpp"
#include "Grid.hpp"
#include "Tile.hpp"
#include <utility>

Line::Line(Grid* grid, LineOrientation orientation, const short index)
    : Region(index, RegionType::LINE, grid)
    , m_orientation(orientation)
{

    const auto lamb = [&](const std::shared_ptr<Tile>& tile) {
        tile->setLine(orientation, this);
    };

    m_elementList = Region::make_line(orientation, index, grid->getGridTiles(), lamb);
}

Line::Line(Line&& other)
    : Region(other)
{
    m_index = other.m_index;
    m_orientation = other.m_orientation;

    auto* head = m_elementList->getHead();
    do {
        auto& tile = head->getElement();
        tile->setLine(m_orientation, this);
    } while ((head = head->next()));

    other.m_elementList = nullptr;
}

Line& Line::operator=(Line&& other)
{
    if (&other == this)
        return *this;

    Region::operator=(std::move(other));

    m_index = other.m_index;
    m_orientation = other.m_orientation;

    auto* head = m_elementList->getHead();
    do {
        auto& tile = head->getElement();
        tile->setLine(m_orientation, this);
    } while ((head = head->next()));

    other.m_elementList = nullptr;

    return *this;
}
