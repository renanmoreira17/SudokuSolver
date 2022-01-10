#include "SinglesTile.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"

#include <algorithm>
#include <iterator>

bool SinglesTile::analyze()
{
    return true;
}

bool SinglesTile::perform()
{
    bool performed = false;
    for (const auto& tile : m_solver.getGridTiles())
    {
        if (tile->hasValue())
        {
            continue;
        }

        const auto& solverTile = std::dynamic_pointer_cast<SolverTile>(tile);

        const auto& tileSuggestions = solverTile->getSuggestions();
        if (tileSuggestions.size() == 1)
        {
            const auto& value = *tileSuggestions.cbegin();
            solverTile->setValue(value);
            performed = true;

            m_solver.report(
                "Singles-Tile:\nO Tile \"{}\" apresenta somente 1 sugestão, no valor {}. Dessa "
                "forma, esse Tile foi definido com esse valor.",
                *solverTile,
                value);
        }
    }

    return performed;
}