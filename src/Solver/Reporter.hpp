#ifndef __REPORTER_H__
#define __REPORTER_H__

#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"
#include "Util/UtilFunctions.hpp"

#include "SolverRegions.hpp"
#include "SolverTile.hpp"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <string>

auto ss = FMT_COMPILE("{}");

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
    void report(const char* a, Args&&... args) const
    {
        const auto formatedMessage = fmt::format(fmt::runtime(a), std::forward<Args>(args)...);
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
struct fmt::formatter<Tile> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Tile& tile, FormatContext& ctx)
    {
        return format_to(ctx.out(),
                         "{}{}",
                         convertRowToLetter(tile.getCoordinates().row),
                         tile.getCoordinates().col + 1);
    }
};

template<>
struct fmt::formatter<SolverTile> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverTile& solverTile, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", static_cast<const Tile&>(solverTile));
    }
};

template<>
struct fmt::formatter<Line> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Line& line, FormatContext& ctx)
    {
        if (line.getLineOrientation() == LineOrientation::VERTICAL)
        {
            return format_to(ctx.out(), "{}", line.getIndex() + 1);
        }
        else
        {
            return format_to(ctx.out(), "{}", convertRowToLetter(line.getIndex()));
        }
    }
};

template<>
struct fmt::formatter<SolverLine> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverLine& solverLine, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", static_cast<const Line&>(solverLine));
    }
};

template<>
struct fmt::formatter<Subgrid> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Subgrid& subgrid, FormatContext& ctx)
    {
        const short subgridIndex = subgrid.getIndex();
        const short subgridRow = subgridIndex / 3;
        const short subgridCol = subgridIndex % 3;
        return format_to(ctx.out(), "{}{}", convertRowToLetter(subgridRow), subgridCol + 1);
    }
};

template<>
struct fmt::formatter<SolverSubgrid> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverSubgrid& solverSubgrid, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", static_cast<const Subgrid&>(solverSubgrid));
    }
};

template<>
struct fmt::formatter<Region> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const Region& region, FormatContext& ctx)
    {
        switch (region.getType())
        {
        case Region::RegionType::LINE:
            return format_to(ctx.out(), "Linha {}", dynamic_cast<const Line&>(region));
        case Region::RegionType::SUBGRID:
            return format_to(ctx.out(), "Quadrado {}", dynamic_cast<const Subgrid&>(region));
        }
    }
};

template<>
struct fmt::formatter<SolverRegion> : formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverRegion& solverRegion, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", static_cast<const Region&>(solverRegion));
    }
};


#endif // __REPORTER_H__
