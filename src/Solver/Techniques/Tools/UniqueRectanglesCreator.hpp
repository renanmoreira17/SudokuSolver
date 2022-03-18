#ifndef __UNIQUERECTANGLESCREATOR_H__
#define __UNIQUERECTANGLESCREATOR_H__

#include "Solver/SolverTypes.hpp"
#include "Util/GlobalDefinitions.hpp"

#include <memory>
#include <vector>

class SolverLine;

class UniqueRectanglesCreator
{
  private:
    const std::vector<TileValueType> m_biValueSuggestions;
    const std::shared_ptr<SolverLine> m_vLine;

  public:
    UniqueRectanglesCreator(std::vector<TileValueType> biValueSuggestions, std::shared_ptr<SolverLine> vLine);

    std::vector<SolverTileVec> buildPossibleRectangles() const;
};

#endif // __UNIQUERECTANGLESCREATOR_H__