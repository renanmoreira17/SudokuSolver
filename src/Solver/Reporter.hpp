#ifndef __REPORTER_H__
#define __REPORTER_H__

#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"
#include "Util/UtilFunctions.hpp"

#include "SolverRegions.hpp"
#include "SolverTile.hpp"

#include <format>
#include <string>

enum ReportMessage
{
    TEST_TILE,
    TEST_TILE2
};

constexpr const char* getFmtStringFor(ReportMessage messageType)
{
    switch (messageType)
    {
    case ReportMessage::TEST_TILE: return "{}";
    case ReportMessage::TEST_TILE2: return "{}";
    }
}

class Reporter
{
  public:
    using ReporterFunc = std::function<void(const std::string&)>;

    template<typename... Args>
    void report(const std::format_string<Args...>& fmt, Args&&... args) const
    {
        const auto formatedMessage = std::vformat(fmt.get(), std::make_format_args(args...));
        m_reporterFunc(formatedMessage);
    }

    Reporter(ReporterFunc reporterFunc)
        : m_reporterFunc(reporterFunc)
    {}
    ~Reporter() = default;

  private:
    ReporterFunc m_reporterFunc;
};

template<>
struct std::formatter<Tile> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Tile& tile, FormatContext& ctx) const
    {
        return std::format_to(
            ctx.out(), "{}{}", convertRowToLetter(tile.getCoordinates().row), tile.getCoordinates().col + 1);
    }
};

template<>
struct std::formatter<SolverTile> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverTile& solverTile, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", static_cast<const Tile&>(solverTile));
    }
};

template<>
struct std::formatter<std::shared_ptr<SolverTile>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const std::shared_ptr<SolverTile>& solverTile, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", *solverTile);
    }
};

template<>
struct std::formatter<Line> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Line& line, FormatContext& ctx) const
    {
        if (line.getLineOrientation() == LineOrientation::VERTICAL)
        {
            return std::format_to(ctx.out(), "{}", line.getIndex() + 1);
        }
        else
        {
            return std::format_to(ctx.out(), "{}", convertRowToLetter(line.getIndex()));
        }
    }
};

template<>
struct std::formatter<SolverLine> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverLine& solverLine, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", static_cast<const Line&>(solverLine));
    }
};

template<>
struct std::formatter<std::shared_ptr<SolverLine>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const std::shared_ptr<SolverLine>& solverLine, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", *solverLine);
    }
};

template<>
struct std::formatter<Subgrid> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Subgrid& subgrid, FormatContext& ctx) const
    {
        const short subgridIndex = subgrid.getIndex();
        const short subgridRow = subgridIndex / 3;
        const short subgridCol = subgridIndex % 3;
        return std::format_to(ctx.out(), "{}{}", convertRowToLetter(subgridRow), subgridCol + 1);
    }
};

template<>
struct std::formatter<SolverSubgrid> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverSubgrid& solverSubgrid, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", static_cast<const Subgrid&>(solverSubgrid));
    }
};

template<>
struct std::formatter<std::shared_ptr<SolverSubgrid>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const std::shared_ptr<SolverSubgrid>& solverSubgrid, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", *solverSubgrid);
    }
};

template<>
struct std::formatter<Region> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Region& region, FormatContext& ctx) const
    {
        switch (region.getType())
        {
        case Region::RegionType::LINE:
            return std::format_to(ctx.out(), "Linha {}", dynamic_cast<const Line&>(region));
        case Region::RegionType::SUBGRID:
            return std::format_to(ctx.out(), "Quadrado {}", dynamic_cast<const Subgrid&>(region));
        }
    }
};

template<>
struct std::formatter<SolverRegion> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverRegion& solverRegion, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", static_cast<const Region&>(solverRegion));
    }
};

template<>
struct std::formatter<std::shared_ptr<SolverRegion>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const std::shared_ptr<SolverRegion>& solverRegion, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", *solverRegion);
    }
};

#endif // __REPORTER_H__
