#include "SolverTile.hpp"

#include "Solver.hpp"

SolverTile::SolverTile(Grid* grid, TileValueType row, TileValueType col)
    : Tile(grid, Coordinates{row, col})
{}

void SolverTile::setValue(TileValueType value)
{
    Tile::setValue(value);
    // removera todas as sugestoes desse tile. Com isso, também é necessario decrementar
    // o número de sugestoes para cada valor contido nas sugestoes em cada region, depois
    // limpando as sugestoes
    const auto& regions = getRegions();
    for (const auto& suggestion : m_suggestions)
    {
        // iterate over all regions and cast each region to a SolverRegion
        for (const auto& region : regions)
        {
            auto solverRegion = dynamic_cast<SolverRegion*>(region);
            solverRegion->suggestionRemoved(suggestion);
        }
    }
    // remove todas as sugestões
    m_suggestions.clear();

    // TODO: talvez não passar novamente por tiles já visitados
    // para cada region do tile a ter o valor inserido, remove todas as ocorrência de suggestion
    // desse valor (já que agora ele existe no tile atual)
    for (auto region : regions)
    {
        // para cada tile da region a ser iterada
        // não passara pelo tile que está sendo setado porque agora ele contem um valor.
        for (const auto& tile : *region)
        {
            // cast tile to std::shared_ptr<SolverTile>
            auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
            if (solverTile->hasValue())
                continue;
            const auto erased = solverTile->m_suggestions.erase(value);
            // se tiver apagado (o tile tinha suggestion desse value), diminui o numero de
            // suggestions das SolverRegions desse valor
            if (erased)
            {
                const auto& tileSolverRegions = solverTile->getRegions();
                // iterate over all regions, cast each region to a SolverRegion, and remove the
                // suggestion
                for (const auto& tileSolverRegion : tileSolverRegions)
                {
                    auto* solverRegion = dynamic_cast<SolverRegion*>(tileSolverRegion);
                    solverRegion->suggestionRemoved(value);
                }
            }
        }
    }
}

void SolverTile::computeSuggestions(const bool clear)
{
    auto& [hLine, vLine, sGrid] = getRegionsTuple();
    if (clear)
    {
        m_suggestions.clear();
    }

    for (TileValueType value = 1; value <= 9; ++value)
    {
        if (vLine->hasValue(value) || hLine->hasValue(value) || sGrid->hasValue(value))
            continue;

        const auto [_, inserted] = m_suggestions.insert(value);
        // se tiver inserido, então incrementa em 1 o número de ocorrências dessa sugestão em todas
        // as regions desse tile
        if (inserted)
        {
            const auto& regions = getRegions();
            for (auto&& region : regions)
            {
                auto* solverRegion = dynamic_cast<SolverRegion*>(region);
                solverRegion->suggestionAdded(value);
            }
        }
    }
}

bool SolverTile::canPlaceValueInTile(const TileValueType value, const bool forceCheck) const
{
    if (forceCheck)
    {
        const auto& regions = getRegions();
        return std::none_of(
            regions.begin(), regions.end(), [=](auto&& region) { return region->hasValue(value); });
    }
    else
    {
        return m_suggestions.find(value) == m_suggestions.cend();
    }
}
