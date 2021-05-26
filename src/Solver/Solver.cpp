#include "Solver.hpp"
#include "Board/Tile.hpp"
#include <utility>
#include <algorithm>
#include <array>
#include "Techniques/Singles.hpp"
#include "Techniques/NakedPairs.hpp"

std::set<TileValueType> &Solver::getSuggestions(TileValueType row, TileValueType col)
{
    return m_suggestions(row, col).second;
}

std::set<TileValueType> &Solver::getSuggestions(const Tile &tile)
{
    const auto &coord = tile.getCoordinates();
    return getSuggestions(coord.row, coord.col);
}

const std::set<TileValueType> &Solver::getSuggestions(TileValueType row, TileValueType col) const
{
    return m_suggestions(row, col).second;
}

const std::set<TileValueType> &Solver::getSuggestions(const Tile &tile) const
{
    const auto &coord = tile.getCoordinates();
    return getSuggestions(coord.row, coord.col);
}

SolverLine& Solver::getSolverHorizontalLine(const Tile& tile)
{
    return m_horizontalLines[tile.getHorizontalLine().getIndex()];
}

const SolverLine& Solver::getSolverHorizontalLine(const Tile& tile) const
{
    return m_horizontalLines[tile.getHorizontalLine().getIndex()];
}

SolverLine& Solver::getSolverVerticalLine(const Tile& tile)
{
    return m_verticalLines[tile.getVerticalLine().getIndex()];
}

const SolverLine& Solver::getSolverVerticalLine(const Tile& tile) const
{
    return m_verticalLines[tile.getVerticalLine().getIndex()];
}

SolverSubgrid& Solver::getSolverSubgrid(const Tile& tile)
{
    return m_subgrids[tile.getSubgrid().getIndex()];
}

const SolverSubgrid& Solver::getSolverSubgrid(const Tile& tile) const
{
    return m_subgrids[tile.getSubgrid().getIndex()];
}

const std::tuple<SolverLine&, SolverLine&, SolverSubgrid&> Solver::getSolverRegionsTuple(const Tile& tile)
{
    return std::tie(getSolverHorizontalLine(tile), getSolverVerticalLine(tile), getSolverSubgrid(tile));
}

const std::tuple<const SolverLine&, const SolverLine&, const SolverSubgrid&> Solver::getSolverRegionsTuple(const Tile& tile) const
{
    return std::tie(getSolverHorizontalLine(tile), getSolverVerticalLine(tile), getSolverSubgrid(tile));
}

const std::array<SolverRegion* const, 3> Solver::getSolverRegions(const Tile& tile)
{
    return {&getSolverHorizontalLine(tile), &getSolverVerticalLine(tile), &getSolverSubgrid(tile)};
}

const std::array<const SolverRegion* const, 3> Solver::getSolverRegions(const Tile& tile) const
{
    return {&getSolverHorizontalLine(tile), &getSolverVerticalLine(tile), &getSolverSubgrid(tile)};
}

void Solver::computeAllSuggestions(const bool clear)
{
    for (auto &tile : m_grid->m_gridTiles)
    {
        if (tile.hasValue())
            continue;

        computeTileSuggestions(tile, clear);
    }
}

void Solver::computeTileSuggestions(const Tile &tile, const bool clear)
{
    auto& [hLine, vLine, sGrid] = tile.getRegionsTuple();
    if (clear)
    {
        getSuggestions(tile).clear();
    }

    for (TileValueType value = 1; value <= 9; value++)
    {
        if (vLine.hasValue(value) || hLine.hasValue(value) || sGrid.hasValue(value))
            continue;
        bool inserted;
        std::tie(std::ignore, inserted) = getSuggestions(tile).insert(value);
        // se tiver inserido, então incrementa em 1 o número de ocorrências dessa sugestão em todas
        // as regions desse tile
        if (inserted)
        {
            auto& regions = getSolverRegions(tile);
            for (auto &&region : regions)
            {
                region->suggestionAdded(value);
            }
        }
    }
}

bool Solver::canPlaceValueInTile(const Tile &tile, const TileValueType value, const bool forceCheck) const
{
    if (forceCheck)
    {
        const auto& regions = tile.getRegions();
        return std::none_of(regions.begin(), regions.end(), [=](auto&& region){ return (**region).hasValue(value);});
    }
    else
    {
        const auto &suggestion = getSuggestions(tile);
        return suggestion.find(value) == suggestion.cend();
    }
}

void Solver::placeValueInTile(Tile* const& tile, const TileValueType value)
{
    placeValueInTile(*tile, value);
}

void Solver::placeValueInTile(const Tile* const& tile, const TileValueType value)
{
    placeValueInTile(*tile, value);
}

void Solver::placeValueInTile(const Tile &tile, const TileValueType value)
{
    const auto &coord = tile.getCoordinates();
    Tile &tile_mut = (*m_grid)(coord.row, coord.col);
    placeValueInTile(tile_mut, value);
}

void Solver::placeValueInTile(Tile &tile, const TileValueType value)
{
    tile.setValue(value);
    // removera todas as sugestoes desse tile. Com isso, também é necessario decrementar
    // o número de sugestoes para cada valor contido nas sugestoes em cada region, depois
    // limpando as sugestoes
    auto& solverRegions = getSolverRegions(tile);
    for (auto &&suggestion : getSuggestions(tile))
    {
        for (auto &&solverRegion : solverRegions)
        {
            solverRegion->suggestionRemoved(suggestion);
        }
    }
    // remove todas as sugestões
    getSuggestions(tile).clear();

    // TODO: talvez não passar novamente por tiles já visitados
    // para cada region do tile a ter o valor inserido, remove todas as ocorrência de suggestion
    // desse valor (já que agora ele existe no tile atual)
    for (auto &&region : tile.getRegions())
    {
        // para cada tile da region a ser iterada
        // não passara pelo tile que está sendo setado porque agora ele contem um valor.
        for (auto &tile_i : **region)
        {
            if (tile_i.hasValue())
                continue;
            const auto erased = getSuggestions(tile_i).erase(value);
            // se tiver apagado (o tile tinha suggestion desse value), diminui o numero de suggestions das
            // SolverRegions desse valor
            if (erased)
            {
                const auto& solverRegions = getSolverRegions(tile_i);
                for (auto &&solverRegion : solverRegions)
                {
                    solverRegion->suggestionRemoved(value);
                }
            }
        }
    }
}

Solver::Solver(Grid *const &grid)
: m_grid(grid)
{
    initializeTechniques();

    auto it = m_suggestions.begin();
    for (auto &&tile : grid->getGridTiles())
    {
        it->first = &tile;
        it++;
    }

    for (TileValueType i = 0; i < 9; i++)
    {
        m_horizontalLines[i] = SolverLine(grid->getHorizontalLine(i), this);
        m_verticalLines[i] = SolverLine(grid->getVerticalLine(i), this);
        m_subgrids[i] = SolverSubgrid(grid->getSubgrid(i), this);

        m_allRegions[i*3+0] = &m_horizontalLines[i];
        m_allRegions[i*3+1] = &m_verticalLines[i];
        m_allRegions[i*3+2] = &m_subgrids[i];
    }
    // std::copy(&m_horizontalLines, &m_horizontalLines + 9*sizeof(SolverLine), std::begin(m_allRegions));
    // std::copy(&m_verticalLines, &m_verticalLines + 9*sizeof(SolverLine), std::end(m_allRegions));
    // std::copy(&m_subgrids, &m_subgrids + 9*sizeof(SolverSubgrid), std::end(m_allRegions));
    // m_allRegions.insert(m_allRegions.end(), m_verticalLines, m_verticalLines + 9);
    // m_allRegions.insert(m_allRegions.end(), m_horizontalLines, m_horizontalLines + 9);
    // m_allRegions.insert(m_allRegions.end(), m_subgrids, m_subgrids + 9);
}

void Solver::solve()
{
    computeAllSuggestions();
    // checar enquanto o board não tiver solucionado
    while (true) {
        for (auto &&technique : m_techniques)
        {
            if (technique->run())
                break;
            //checar se solucionou
        }
    }
}

void Solver::initializeTechniques()
{
    m_techniques.emplace_back(std::make_unique<NakedPairs>(this));
    m_techniques.emplace_back(std::make_unique<Singles>(this));
}
