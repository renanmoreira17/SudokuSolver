#ifndef __GRIDPRINTER_H__
#define __GRIDPRINTER_H__

#include  <string>

class Grid;

class GridPrinter
{
  public:
    GridPrinter(const Grid& grid);
    virtual ~GridPrinter() = default;

    std::string createBoardString() const;

  private:
    const Grid& m_grid;
};

#endif // __GRIDPRINTER_H__