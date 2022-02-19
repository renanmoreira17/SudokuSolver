#include "Subgrid.hpp"

#include "ElementList.hpp"
#include "ElementWrapper.hpp"
#include "Grid.hpp"
#include "Tile.hpp"
#include <utility>

Subgrid::Subgrid(Grid* grid, const short index)
    : Region(index, RegionType::SUBGRID, grid)
{
    const auto lambda = [&](const std::shared_ptr<Tile>& tile) {
        tile->setSubgrid(this);
    };
    m_elementList = Region::make_subgrid(index, grid->getGridTiles(), lambda);
}

Subgrid::Subgrid(Subgrid&& other)
    : Region(std::move(other))
{
    m_index = other.m_index;

    auto* head = m_elementList->getHead();
    do {
        head->getElement()->setSubgrid(this);
    } while ((head = head->next()));

    other.m_elementList = nullptr;
}

Subgrid& Subgrid::operator=(Subgrid&& other)
{
    if (&other == this)
        return *this;

    Region::operator=(std::move(other));

    m_index = other.m_index;

    auto head = m_elementList->getHead();
    do {
        head->getElement()->setSubgrid(this);
    } while ((head = head->next()));

    other.m_elementList = nullptr;

    return *this;
}

Subgrid::Subgrid(const Subgrid& other, Grid* grid)
    : Subgrid(grid, other.getIndex())
{}

const std::shared_ptr<Tile> Subgrid::getTile(const TileValueType row, const TileValueType col) const
{
    const auto first = m_elementList->cbegin();
    return *std::next(first, row * 3 + col);
}

const std::shared_ptr<Tile> Subgrid::operator()(const TileValueType row, const TileValueType col) const
{
    return getTile(row, col);
}
