#include "Subgrid.hpp"
#include "ElementWrapper.hpp"
#include "ElementList.hpp"
#include "Grid.hpp"
#include <utility>
#include "Tile.hpp"

Subgrid::Subgrid(Grid* grid, const short index)
: Region(index)
, m_grid(grid)
{
    const auto lambda = [&](Tile& tile) {tile.setSubgrid(this);};
    m_elementList = Region<Tile>::make_subgrid(index, grid->getGridTiles(), std::make_optional(lambda));
}

Subgrid::Subgrid(Subgrid&& other)
: Region(other)
{
    m_grid = other.m_grid;
    m_index = other.m_index;

    auto* head = m_elementList->getHead();
    do
    {
        head->getElement().setSubgrid(this);
    } while ((head = head->next()));

    other.m_grid = nullptr;
    other.m_elementList = nullptr;
}

Subgrid& Subgrid::operator=(Subgrid&& other)
{
    if (&other == this) return *this;

    Region::operator=(std::move(other));

    m_grid = other.m_grid;
    m_index = other.m_index;

    auto head = m_elementList->getHead();
    do
    {
        head->getElement().setSubgrid(this);
    } while ((head = head->next()));

    other.m_grid = nullptr;
    other.m_elementList = nullptr;

    return *this;
}
