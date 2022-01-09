#include "GridPrinter.hpp"

#include "GlobalDefinitions.hpp"
#include "Grid.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
std::string createSubgridDisplayStringFromTileDisplayStrings(
    const std::vector<std::vector<std::string>>& tileDisplayStrings)
{
    const static std::string edgeString = "+";
    const static std::string verticalLineString = "|";
    const static std::string horizontalBorderString = edgeString + std::string(5, '-') +
                                                      edgeString + std::string(5, '-') +
                                                      edgeString + std::string(5, '-') + edgeString;

    std::string subgridDisplayString;
    for (const auto& row : tileDisplayStrings)
    {
        subgridDisplayString += horizontalBorderString + "\n";
        for (const auto& tileDisplayString : row) { subgridDisplayString += tileDisplayString; }
        subgridDisplayString += "\n";
    }
    return subgridDisplayString;
}

class MultilineElement
{
  public:
    MultilineElement() = default;
    MultilineElement(std::vector<std::string>&& lines)
        : m_lines(std::move(lines))
    {}

    const std::vector<std::string>& getLines() const { return m_lines; }
    const std::string& getLine(const short index) const { return m_lines[index]; }

    void addLine(const std::string& line) { m_lines.emplace_back(line); }
    void addLine(std::string&& line) { m_lines.emplace_back(std::move(line)); }

    std::string build() const
    {
        std::stringstream multilineString;
        for (const auto& line : m_lines) { multilineString << line + "\n"; }
        return multilineString.str();
    }

  private:
    std::vector<std::string> m_lines;
};

class DisplayMultilineElementBuilder
{
  public:
    DisplayMultilineElementBuilder(unsigned short linesNumber)
        : m_linesNumber(linesNumber)
    {}
    DisplayMultilineElementBuilder(std::vector<MultilineElement>&& elements,
                                   unsigned short linesNumber)
        : m_linesNumber(linesNumber)
        , m_elements(std::move(elements))
    {}

    // setters
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
        for (TileValueType row = 0; row < 3; ++row)
        {
            if (m_shouldAddCustomLineBreak)
            {
                rowLines.emplace_back(m_customLineBreak);
            }
            for (unsigned short lineNumber = 0; lineNumber < m_linesNumber; ++lineNumber)
            {
                std::string currentLine{};
                for (TileValueType col = 0; col < 3; ++col)
                {
                    if (m_shouldAddCustomColumnDelimitator)
                    {
                        currentLine += m_customColumnDelimitator;
                    }
                    currentLine += m_elements[row * 3 + col].getLine(lineNumber);
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

        return {std::move(rowLines)};
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

class DisplayGridBuilder
{
  public:
    DisplayGridBuilder() = default;
    DisplayGridBuilder(const Grid& grid)
        : m_grid(grid)
    {}

    void build();

    const MultilineElement& getDisplayGrid() const { return m_displayGrid; }

  private:
    const Grid& m_grid;
    MultilineElement m_displayGrid;
};

} // namespace

GridPrinter::GridPrinter(const Grid& grid)
    : m_grid(grid)
{}

void GridPrinter::print() const
{
    std::vector<MultilineElement> subgridDisplayElements;
    for (const auto& subgrid : m_grid.getSubgrids())
    {
        std::vector<MultilineElement> currentSubgridDisplayElements;
        for (TileValueType row = 0; row < 3; ++row)
        {
            for (TileValueType col = 0; col < 3; ++col)
            {
                const auto& subgridTileCoord = subgrid->getTile(row, col)->getCoordinates();
                currentSubgridDisplayElements.emplace_back(
                    m_grid.requestTileDisplayStringForCoordinate(subgridTileCoord.row,
                                                                 subgridTileCoord.col));
            }
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

    const MultilineElement boardDisplayElements = boardElementsBuilder.build();
    std::cout << boardDisplayElements.build() << std::endl;
}
