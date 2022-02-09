#ifndef __SUBGRID_H__
#define __SUBGRID_H__

#include "Region.hpp"

class Grid;
class Tile;

class Subgrid : virtual public Region
{
  private:
    Grid* m_grid;

  public:
    Subgrid(Grid* grid, const short index);
    Subgrid(const Subgrid& other) = default;
    Subgrid(Subgrid&& other);
    Subgrid& operator=(const Subgrid& other) = default;
    Subgrid& operator=(Subgrid&& other);
    ~Subgrid() {}

    RegionSpecificType getRegionSpecificType() const override
    {
        return RegionSpecificType::SUBGRID;
    }

    const std::shared_ptr<Tile> getTile(const TileValueType row, const TileValueType col) const;
    const std::shared_ptr<Tile> operator()(const TileValueType row, const TileValueType col) const;
};

#endif // __SUBGRID_H__
