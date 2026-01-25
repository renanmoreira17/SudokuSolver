#ifndef __REPORTER_H__
#define __REPORTER_H__

#include "Util/UtilFunctions.hpp"

#include "SolverRegions.hpp"
#include "SolverTile.hpp"

#include <format>
#include <functional>
#include <string>
#include <string_view>

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
struct std::formatter<SolverTile> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverTile& solverTile, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(),
                              "{}{}",
                              convertRowToLetter(solverTile.getCoordinates().row),
                              solverTile.getCoordinates().col + 1);
    }
};

template<>
struct std::formatter<SolverLine> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverLine& solverLine, FormatContext& ctx) const
    {
        if (solverLine.getLineOrientation() == LineOrientation::VERTICAL)
        {
            return std::format_to(ctx.out(), "{}", solverLine.getIndex() + 1);
        }
        return std::format_to(ctx.out(), "{}", convertRowToLetter(solverLine.getIndex()));
    }
};

template<>
struct std::formatter<SolverSubgrid> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverSubgrid& solverSubgrid, FormatContext& ctx) const
    {
        const short subgridIndex = solverSubgrid.getIndex();
        const short subgridRow = subgridIndex / 3;
        const short subgridCol = subgridIndex % 3;
        return std::format_to(ctx.out(), "{}{}", convertRowToLetter(subgridRow), subgridCol + 1);
    }
};

template<>
struct std::formatter<SolverRegion> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const SolverRegion& solverRegion, FormatContext& ctx) const
    {
        switch (solverRegion.getRegionSpecificType())
        {
        case RegionSpecificType::HORIZONTAL_LINE:
            return std::format_to(ctx.out(),
                                  "Linha {}",
                                  convertRowToLetter(solverRegion.getIndex()));
        case RegionSpecificType::VERTICAL_LINE:
            return std::format_to(ctx.out(), "Linha {}", solverRegion.getIndex() + 1);
        case RegionSpecificType::SUBGRID:
        {
            const short subgridRow = solverRegion.getIndex() / 3;
            const short subgridCol = solverRegion.getIndex() % 3;
            return std::format_to(
                ctx.out(), "Quadrado {}{}", convertRowToLetter(subgridRow), subgridCol + 1);
        }
        default:
            return std::format_to(ctx.out(), "Regiao {}", solverRegion.getIndex());
        }
    }
};

#endif // __REPORTER_H__
