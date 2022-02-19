#ifndef __SUBGRID_H__
#define __SUBGRID_H__

#include "Region.hpp"

class Grid;
class Tile;

class Subgrid : virtual public Region
{
  public:
    Subgrid(Grid* grid, const short index);
    Subgrid(Subgrid&& other);
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
