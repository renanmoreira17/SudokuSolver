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
};

#endif // __SUBGRID_H__
