#include "Solver.hpp"

#include "SolverRegions.hpp"
#include "SolverTile.hpp"
#include "SolverUtils.hpp"
#include "Technique.hpp"
#include "Techniques/HiddenPairs.hpp"
#include "Techniques/HiddenUniqueRectangles.hpp"
#include "Techniques/NakedPairs.hpp"
#include "Techniques/NakedTriples.hpp"
#include "Techniques/PointingPair.hpp"
#include "Techniques/SinglesChains.hpp"
#include "Techniques/SinglesRegion.hpp"
#include "Techniques/SinglesTile.hpp"
#include "Techniques/UniqueRectangles.hpp"

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace
{
constexpr TileValueType kGridSize = 9;
constexpr TileValueType kSubgridSize = 3;

class MultilineElement
{
  public:
    MultilineElement() = default;
    explicit MultilineElement(std::vector<std::string>&& lines)
        : m_lines(std::move(lines))
    {}

    const std::vector<std::string>& getLines() const { return m_lines; }
    const std::string& getLine(const short index) const { return m_lines[index]; }

    void addLine(const std::string& line) { m_lines.emplace_back(line); }
    void addLine(std::string&& line) { m_lines.emplace_back(std::move(line)); }
    void addLineToBeginning(const std::string& line) { m_lines.insert(m_lines.begin(), line); }
    void addFromMultilineElement(const MultilineElement& multilineElement)
    {
        m_lines.insert(
            m_lines.end(), multilineElement.getLines().begin(), multilineElement.getLines().end());
    }

    std::string build() const
    {
        std::stringstream multilineString;
        for (const auto& line : m_lines) { multilineString << line + "\n"; }
        return multilineString.str();
    }

    MultilineElement operator+(const MultilineElement& other) const
    {
        if (m_lines.size() != other.m_lines.size())
        {
            throw std::runtime_error("MultilineElement size mismatch while composing board.");
        }
        MultilineElement result;
        for (size_t i = 0; i < m_lines.size(); ++i)
        {
            result.addLine(m_lines[i] + other.m_lines[i]);
        }
        return result;
    }

  private:
    std::vector<std::string> m_lines;
};

class DisplayMultilineElementBuilder
{
  public:
    explicit DisplayMultilineElementBuilder(unsigned short linesNumber)
        : m_linesNumber(linesNumber)
    {}
    DisplayMultilineElementBuilder(std::vector<MultilineElement>&& elements,
                                   unsigned short linesNumber)
        : m_linesNumber(linesNumber)
        , m_elements(std::move(elements))
    {}

    void setShouldAddCustomLineBreak(const bool shouldAddCustomLineBreak)
    {
        m_shouldAddCustomLineBreak = shouldAddCustomLineBreak;
    }

    void setCustomLineBreak(const std::string& customLineBreak)
    {
        m_customLineBreak = customLineBreak;
    }

    void setShouldAddCustomColumnDelimitator(const bool shouldAddCustomColumnDelimitator)
    {
        m_shouldAddCustomColumnDelimitator = shouldAddCustomColumnDelimitator;
    }

    void setCustomColumnDelimitator(const std::string& customColumnDelimitator)
    {
        m_customColumnDelimitator = customColumnDelimitator;
    }

    void addMultilineElement(MultilineElement&& element)
    {
        m_elements.emplace_back(std::move(element));
    }

    MultilineElement build() const
    {
        std::vector<std::string> rowLines;
        for (TileValueType row = 0; row < kSubgridSize; ++row)
        {
            if (m_shouldAddCustomLineBreak)
            {
                rowLines.emplace_back(m_customLineBreak);
            }
            for (unsigned short lineNumber = 0; lineNumber < m_linesNumber; ++lineNumber)
            {
                std::string currentLine{};
                for (TileValueType col = 0; col < kSubgridSize; ++col)
                {
                    if (m_shouldAddCustomColumnDelimitator)
                    {
                        currentLine += m_customColumnDelimitator;
                    }
                    currentLine += m_elements[row * kSubgridSize + col].getLine(lineNumber);
                }
                if (m_shouldAddCustomColumnDelimitator)
                {
                    currentLine += m_customColumnDelimitator;
                }
                rowLines.emplace_back(currentLine);
            }
        }
        if (m_shouldAddCustomLineBreak)
        {
            rowLines.emplace_back(m_customLineBreak);
        }

        return MultilineElement(std::move(rowLines));
    }

  private:
    const unsigned short m_linesNumber;

    bool m_shouldAddCustomLineBreak = true;
    std::string m_customLineBreak =
        "+" + std::string(5, '-') + "+" + std::string(5, '-') + "+" + std::string(5, '-') + "+";

    bool m_shouldAddCustomColumnDelimitator = true;
    std::string m_customColumnDelimitator = "|";

    std::vector<MultilineElement> m_elements;
};

size_t toIndex(TileValueType row, TileValueType col)
{
    return static_cast<size_t>(row) * kGridSize + col;
}

MultilineElement buildRowLabels(const unsigned short tileLineCount)
{
    std::vector<std::string> lines;
    lines.reserve(1 + (kSubgridSize * (1 + kSubgridSize * (tileLineCount + 1))));
    lines.emplace_back("  ");

    TileValueType rowIndex = 0;
    for (TileValueType subgridRow = 0; subgridRow < kSubgridSize; ++subgridRow)
    {
        lines.emplace_back("  ");
        for (TileValueType rowInSubgrid = 0; rowInSubgrid < kSubgridSize; ++rowInSubgrid)
        {
            for (unsigned short lineIndex = 0; lineIndex < tileLineCount; ++lineIndex)
            {
                if (lineIndex == tileLineCount / 2)
                {
                    const char label = static_cast<char>('A' + rowIndex);
                    lines.emplace_back(std::string{label, ' '});
                }
                else
                {
                    lines.emplace_back("  ");
                }
            }
            lines.emplace_back("  ");
            ++rowIndex;
        }
    }

    return MultilineElement(std::move(lines));
}
} // namespace

Solver::Solver()
{
    initializeLayout();
    initializeTechniques();
}

Solver::Solver(const std::string& fromBoard)
{
    initializeLayout();
    initializeFromBoard(fromBoard);
    initializeTechniques();
}

Solver::~Solver() = default;

void Solver::initializeLayout()
{
    m_tiles.clear();
    m_tiles.reserve(kGridSize * kGridSize);
    for (TileValueType row = 0; row < kGridSize; ++row)
    {
        for (TileValueType col = 0; col < kGridSize; ++col)
        {
            m_tiles.emplace_back(std::make_shared<SolverTile>(row, col));
        }
    }

    m_horizontalLines.clear();
    m_verticalLines.clear();
    m_subgrids.clear();
    m_allRegions.clear();

    m_horizontalLines.reserve(kGridSize);
    m_verticalLines.reserve(kGridSize);
    m_subgrids.reserve(kGridSize);
    m_allRegions.reserve(3 * kGridSize);

    for (TileValueType index = 0; index < kGridSize; ++index)
    {
        SolverTileVec tiles;
        tiles.reserve(kGridSize);
        for (TileValueType col = 0; col < kGridSize; ++col)
        {
            tiles.emplace_back(getTileAt(index, col));
        }
        auto line = std::make_shared<SolverLine>(LineOrientation::HORIZONTAL, index, tiles);
        for (const auto& tile : tiles)
        {
            tile->setHorizontalLine(line.get());
        }
        m_horizontalLines.emplace_back(std::move(line));
    }

    for (TileValueType index = 0; index < kGridSize; ++index)
    {
        SolverTileVec tiles;
        tiles.reserve(kGridSize);
        for (TileValueType row = 0; row < kGridSize; ++row)
        {
            tiles.emplace_back(getTileAt(row, index));
        }
        auto line = std::make_shared<SolverLine>(LineOrientation::VERTICAL, index, tiles);
        for (const auto& tile : tiles)
        {
            tile->setVerticalLine(line.get());
        }
        m_verticalLines.emplace_back(std::move(line));
    }

    for (TileValueType index = 0; index < kGridSize; ++index)
    {
        const TileValueType subgridRow = index / kSubgridSize;
        const TileValueType subgridCol = index % kSubgridSize;
        SolverTileVec tiles;
        tiles.reserve(kGridSize);
        for (TileValueType row = 0; row < kSubgridSize; ++row)
        {
            for (TileValueType col = 0; col < kSubgridSize; ++col)
            {
                tiles.emplace_back(
                    getTileAt(subgridRow * kSubgridSize + row, subgridCol * kSubgridSize + col));
            }
        }
        auto subgrid = std::make_shared<SolverSubgrid>(index, tiles);
        for (const auto& tile : tiles)
        {
            tile->setSubgrid(subgrid.get());
        }
        m_subgrids.emplace_back(std::move(subgrid));
    }

    for (const auto& subgrid : m_subgrids)
    {
        m_allRegions.emplace_back(subgrid);
    }
    for (const auto& line : m_horizontalLines)
    {
        m_allRegions.emplace_back(line);
    }
    for (const auto& line : m_verticalLines)
    {
        m_allRegions.emplace_back(line);
    }
}

void Solver::initializeFromBoard(const std::string& fromBoard)
{
    size_t currentIndex = 0;
    size_t currentTileIndex = 0;
    while (currentIndex < fromBoard.size() && currentTileIndex < (kGridSize * kGridSize))
    {
        const auto& currentTile = this->operator()(currentTileIndex / kGridSize,
                                                   currentTileIndex % kGridSize);
        ++currentTileIndex;
        const auto& currentToken = fromBoard[currentIndex];
        switch (currentToken)
        {
        case '{':
        {
            m_initializedWithSuggestions = true;
            const auto endingSuggestionIndex = fromBoard.find('}', currentIndex);
            if (endingSuggestionIndex == std::string::npos)
            {
                throw std::runtime_error(
                    std::format("Missing closing bracket for suggestion at tile index {}", currentTileIndex));
            }
            const auto suggestionsString =
                fromBoard.substr(currentIndex + 1, endingSuggestionIndex - currentIndex - 1);
            for (const auto& suggestion : suggestionsString)
            {
                const auto value = static_cast<TileValueType>(suggestion - '0');
                if (value < 1 || value > 9)
                {
                    throw std::runtime_error(
                        std::format("Invalid suggestion value {} at tile index {}", value, currentTileIndex));
                }
                currentTile->addSuggestion(value);
            }
            currentIndex = endingSuggestionIndex + 1;
            break;
        }
        default:
        {
            ++currentIndex;
            if (currentToken == '.' || currentToken == '0')
            {
                break;
            }
            const auto value = static_cast<TileValueType>(currentToken - '0');
            if (value > 9)
            {
                throw std::runtime_error(
                    std::format("Invalid tile value {} at tile index {}", value, currentTileIndex));
            }
            currentTile->setValue(value);
        }
        }
    }
}

void Solver::computeAllSuggestions(const bool clear)
{
    for (auto& tile : m_tiles)
    {
        if (!tile || tile->hasValue())
        {
            continue;
        }
        tile->computeSuggestions(clear);
    }
}

void Solver::computeTileSuggestions(const SolverTilePtr& tile, const bool clear)
{
    if (!tile || tile->hasValue())
    {
        return;
    }
    tile->computeSuggestions(clear);
}

bool Solver::canPlaceValueInTile(const SolverTile& tile,
                                 const TileValueType value,
                                 const bool forceCheck) const
{
    return tile.canPlaceValueInTile(value, forceCheck);
}

#define INIT_TECHNIQUE(TechniqueType) m_techniques.emplace_back(std::make_unique<TechniqueType>(*this));

void Solver::initializeTechniques()
{
    INIT_TECHNIQUE(SinglesTile);
    INIT_TECHNIQUE(SinglesRegion);
    INIT_TECHNIQUE(PointingPair);
    INIT_TECHNIQUE(HiddenPairs);
    INIT_TECHNIQUE(NakedPairs);
    INIT_TECHNIQUE(NakedTriples);
    INIT_TECHNIQUE(SinglesChains);
    INIT_TECHNIQUE(UniqueRectangles);
    INIT_TECHNIQUE(HiddenUniqueRectangles);
}

SolverTilePtr Solver::getTileAt(TileValueType row, TileValueType col) const
{
    const auto index = toIndex(row, col);
    if (index >= m_tiles.size())
    {
        throw std::out_of_range("Tile coordinates out of range");
    }
    return m_tiles[index];
}

SolverTilePtr Solver::operator()(TileValueType row, TileValueType col) const
{
    return getTileAt(row, col);
}

SolverTilePtr Solver::operator()(const Coordinates& coordinates) const
{
    return getTileAt(coordinates.row, coordinates.col);
}

const std::vector<std::shared_ptr<SolverRegion>>& Solver::getAllRegions() const
{
    return m_allRegions;
}

const std::vector<std::shared_ptr<SolverSubgrid>>& Solver::getAllSolverSubgrids() const
{
    return m_subgrids;
}

const std::vector<std::shared_ptr<SolverLine>>& Solver::getAllSolverHorizontalLines() const
{
    return m_horizontalLines;
}

const std::vector<std::shared_ptr<SolverLine>>& Solver::getAllSolverVerticalLines() const
{
    return m_verticalLines;
}

std::vector<std::string> Solver::requestTileDisplayStringForCoordinate(const TileValueType row,
                                                                       const TileValueType col) const
{
    const auto solverTile = getTileAt(row, col);
    if (solverTile->hasValue())
    {
        std::vector<std::string> result;
        result.emplace_back(R"(\   /)");
        result.emplace_back(
            std::format(R"(  {}  )", solverTile->hasValue() ? std::to_string(solverTile->getValue()) : " "));
        result.emplace_back(R"(/   \)");
        return result;
    }

    std::vector<std::string> result;
    for (TileValueType lineRow = 0; lineRow < kSubgridSize; lineRow++)
    {
        std::stringstream line;
        for (TileValueType lineCol = 0; lineCol < kSubgridSize; lineCol++)
        {
            const auto value = lineRow * kSubgridSize + lineCol + 1;
            if (solverTile->hasSuggestion(value))
            {
                line << value;
            }
            else
            {
                line << " ";
            }

            if (lineCol < 2)
            {
                line << " ";
            }
        }
        result.emplace_back(line.str());
    }

    return result;
}

std::string Solver::getBoardString() const
{
    unsigned short tileLineCount = 0;
    std::vector<MultilineElement> subgridDisplayElements;
    for (TileValueType subgridIndex = 0; subgridIndex < kGridSize; ++subgridIndex)
    {
        std::vector<MultilineElement> currentSubgridDisplayElements;
        const TileValueType subgridRow = subgridIndex / kSubgridSize;
        const TileValueType subgridCol = subgridIndex % kSubgridSize;
        for (TileValueType row = 0; row < kSubgridSize; ++row)
        {
            for (TileValueType col = 0; col < kSubgridSize; ++col)
            {
                const auto absoluteRow = subgridRow * kSubgridSize + row;
                const auto absoluteCol = subgridCol * kSubgridSize + col;
                currentSubgridDisplayElements.emplace_back(
                    requestTileDisplayStringForCoordinate(absoluteRow, absoluteCol));
            }
        }
        if (tileLineCount == 0)
        {
            tileLineCount =
                static_cast<unsigned short>(currentSubgridDisplayElements.front().getLines().size());
        }
        const DisplayMultilineElementBuilder displayMultilineElementBuilder(
            std::move(currentSubgridDisplayElements),
            currentSubgridDisplayElements.front().getLines().size());
        subgridDisplayElements.emplace_back(displayMultilineElementBuilder.build());
    }

    DisplayMultilineElementBuilder boardElementsBuilder(
        std::move(subgridDisplayElements), subgridDisplayElements.front().getLines().size());

    boardElementsBuilder.setShouldAddCustomLineBreak(false);
    boardElementsBuilder.setShouldAddCustomColumnDelimitator(false);

    const static std::string columnsLabelString =
        "   1     2     3      4     5     6      7     8     9   ";
    MultilineElement boardWithColumns({columnsLabelString});

    boardWithColumns.addFromMultilineElement(boardElementsBuilder.build());

    if (tileLineCount == 0)
    {
        throw std::runtime_error("Invalid tile line count while building board.");
    }

    const auto linesLabelString = buildRowLabels(tileLineCount);

    const MultilineElement finalBoardDisplay = linesLabelString + boardWithColumns;

    return finalBoardDisplay.build();
}

void Solver::printGrid() const
{
    std::cout << getBoardString() << std::endl;
}

bool Solver::isSolved() const
{
    return std::all_of(m_tiles.begin(), m_tiles.end(), [](const SolverTilePtr& tile) {
        return tile && tile->hasValue();
    });
}

void Solver::solve()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    if (!m_initializedWithSuggestions)
    {
        computeAllSuggestions();
    }
    printGrid();
    while (!isSolved())
    {
        bool performed = false;
        for (auto&& technique : m_techniques)
        {
            performed = technique->run();
            if (performed)
            {
                break;
            }
        }
        if (!performed)
        {
            std::cout << "\nNao foi possivel resolver o board!" << std::endl;
            return;
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "\nBoard resolvido em "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " milisegundos." << std::endl;

    std::cout << "\nTechniques report:\n";
    for (const auto& technique : m_techniques)
    {
        std::cout << std::format("  {:<20}: {}\n", technique->getTechniqueName(), technique->getRanCount());
    }
}
