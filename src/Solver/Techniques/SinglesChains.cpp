#include "SinglesChains.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverUtils.hpp"

#include <map>
#include <memory>
#include <optional>
#include <unordered_set>

#ifdef DEBUG
#include "Util/UtilFunctions.hpp"
#include <fmt/format.h>
#endif

namespace
{
enum class ElementLinkState
{
    NONE,
    RED,
    BLUE
};

ElementLinkState operator!(const ElementLinkState state)
{
    switch (state)
    {
    case ElementLinkState::RED: return ElementLinkState::BLUE;
    case ElementLinkState::BLUE: return ElementLinkState::RED;
    default: return ElementLinkState::NONE;
    }
}

std::string convertElementLinkStateToString(const ElementLinkState state)
{
    switch (state)
    {
    case ElementLinkState::RED: return "RED";
    case ElementLinkState::BLUE: return "BLUE";
    default: return "NONE";
    }
}

class ChainLinkElement : public std::enable_shared_from_this<ChainLinkElement>
{
  private:
    ChainLinkElement(const std::shared_ptr<SolverTile>& sourceTile)
        : m_sourceTile(sourceTile)
    {}

  public:
    ChainLinkElement() = delete;

    static std::shared_ptr<ChainLinkElement> create(const std::shared_ptr<SolverTile>& sourceTile)
    {
        return std::shared_ptr<ChainLinkElement>(new ChainLinkElement(sourceTile));
    }

    const std::shared_ptr<SolverTile>& getSourceTile() const { return m_sourceTile; }
    const std::vector<std::shared_ptr<ChainLinkElement>>& getLinksTo() const { return m_linksTo; }
    const std::weak_ptr<ChainLinkElement>& getLinkedFrom() const { return m_linkedFrom; }
    RegionSpecificType getLinkType() const { return m_linkType; }
    ElementLinkState getLinkState() const { return m_linkState; }
    void setLinkState(const ElementLinkState state) { m_linkState = state; }

    void linkTo(const std::shared_ptr<ChainLinkElement>& targetTile, RegionSpecificType regionType)
    {
        m_linksTo.emplace_back(targetTile);
        assert(targetTile->m_linkedFrom.expired());
        targetTile->m_linkedFrom = weak_from_this();
        targetTile->m_linkType = regionType;
        targetTile->setLinkState(!getLinkState());
    }

  private:
    std::shared_ptr<SolverTile> m_sourceTile;
    std::vector<std::shared_ptr<ChainLinkElement>> m_linksTo;
    std::weak_ptr<ChainLinkElement> m_linkedFrom;
    RegionSpecificType m_linkType{RegionSpecificType::UNKNOWN};
    ElementLinkState m_linkState{ElementLinkState::NONE};
};

class Chain
{
  public:
    Chain(Solver& solver, const TileValueType targetChainValue)
        : m_solver(solver)
        , m_targetChainValue(targetChainValue)
    {}
    Chain() = delete;

    void buildChain()
    {
        for (const auto& subgrid : m_solver.getAllRegions())
        {
            const auto& suggestionsQuan = subgrid->getSuggestionsQuan();
            if (suggestionsQuan.getSuggestionsQuantityFor(m_targetChainValue) != 2)
            {
                continue;
            }

            const auto tiles = subgrid->getTilesWithSuggestion(m_targetChainValue);
            const auto& rootTile = tiles.front();
            const auto currentRootElement = ChainLinkElement::create(rootTile);
            currentRootElement->setLinkState(ElementLinkState::RED);
            findLinksForElement(currentRootElement, std::nullopt);
            m_rootElements.emplace_back(currentRootElement);

            m_visitedTiles.clear();
        }
#ifdef DEBUG
        printChain();
#endif
    }

  private:
    void findLinksForElement(const std::shared_ptr<ChainLinkElement>& element,
                             const std::optional<std::shared_ptr<ChainLinkElement>>& origin)
    {
        const auto& sourceTile = element->getSourceTile();
        const auto insertedInfo = m_visitedTiles.insert(sourceTile);
        if (!insertedInfo.second)
        {
            return;
        }

        for (const SolverRegion* region : sourceTile->getSolverRegions())
        {
            auto tiles = region->getTilesWithSuggestion(m_targetChainValue);
            if (tiles.size() != 2)
            {
                continue;
            }

            // This skips both the current tile which we're finding links for and the tile
            // which originated this link. If we included the origin tile, we would have
            // a duplicated link.
            // Also, we check if the tile has already been visited. This might be the case if
            // we previously found a link with the tile in a different region of the source tile,
            // and we don't want to make double links
            std::erase_if(
                tiles, [&sourceTile, origin, this](const std::shared_ptr<SolverTile>& tile) {
                    return tile == sourceTile ||
                           (origin.has_value() && tile == origin.value()->getSourceTile()) ||
                           hasVisitedTile(tile);
                });
            if (tiles.empty())
            {
                continue;
            }

            assert(tiles.size() == 1);

            const auto targetElement = ChainLinkElement::create(tiles.front());
            element->linkTo(targetElement, region->getRegionSpecificType());

            findLinksForElement(targetElement, element);
        }
    }

    bool hasVisitedTile(const std::shared_ptr<SolverTile>& tile) const
    {
        return m_visitedTiles.find(tile) != m_visitedTiles.cend();
    }

  public:
    const std::vector<std::shared_ptr<ChainLinkElement>>& getRootElements() const
    {
        return m_rootElements;
    }

    TileValueType getTargetChainValue() const { return m_targetChainValue; }

  private:
    Solver& m_solver;
    const TileValueType m_targetChainValue;
    std::vector<std::shared_ptr<ChainLinkElement>> m_rootElements;
    std::unordered_set<std::shared_ptr<SolverTile>> m_visitedTiles;

#ifdef DEBUG
    void printChain() const
    {
        for (const auto& rootElement : getRootElements())
        {
            fmt::print("Chain: {}\n", m_targetChainValue);
            const auto& rootElementCoord = rootElement->getSourceTile()->getCoordinates();
            fmt::print(
                "Root: {}{}\n", convertRowToLetter(rootElementCoord.row), rootElementCoord.col + 1);
            fmt::print("Links:\n");
            std::function<void(const std::shared_ptr<ChainLinkElement>&, int)> printLinksFor =
                [&](const std::shared_ptr<ChainLinkElement>& element, int depth = 0) {
                    const auto& sourceTileCoord = element->getSourceTile()->getCoordinates();
                    const auto& links = element->getLinksTo();
                    // if (!links.empty())
                    // {
                    //     fmt::print("Entering depth {}\n", depth);
                    // }
                    for (const auto& linkedTo : links)
                    {
                        const auto& targetTileCoord = linkedTo->getSourceTile()->getCoordinates();
                        const std::string depthStr = std::string(depth * 2, ' ');
                        fmt::print("{}{}{}({}) -> {}{}({}) (Linked through: {})\n",
                                   depthStr,
                                   convertRowToLetter(sourceTileCoord.row),
                                   sourceTileCoord.col + 1,
                                   convertElementLinkStateToString(element->getLinkState()),
                                   convertRowToLetter(targetTileCoord.row),
                                   targetTileCoord.col + 1,
                                   convertElementLinkStateToString(linkedTo->getLinkState()),
                                   convertRegionSpecificTypeToString(linkedTo->getLinkType()));
                        printLinksFor(linkedTo, depth + 1);
                    }
                    // if (!links.empty())
                    // {
                    //     fmt::print("Exiting depth {}\n", depth);
                    // }
                };
            printLinksFor(rootElement, 0);
            fmt::print("\n");
        }
    }
#endif // DEBUG
};

using FoundElement = std::optional<std::shared_ptr<ChainLinkElement>>;

template<typename ActionFunction>
static FoundElement
traverseElements(const std::shared_ptr<ChainLinkElement>& element,
                 std::unordered_set<std::shared_ptr<ChainLinkElement>>& traversedElements,
                 ActionFunction&& actionFn)
{
    const auto hasVisitedElement =
        [&traversedElements](const std::shared_ptr<ChainLinkElement>& element) {
            return traversedElements.find(element) != traversedElements.cend();
        };

    if (!hasVisitedElement(element))
    {
        traversedElements.emplace(element);
        const auto result = actionFn(element);
        if (result)
        {
            return element;
        }
    }

    for (const auto& child : element->getLinksTo())
    {
        if (hasVisitedElement(child))
        {
            continue;
        }
        const auto childResult = traverseElements(child, traversedElements, actionFn);
        if (childResult)
        {
            return childResult;
        }
    }
    const auto parent = element->getLinkedFrom().lock();
    if (parent && !hasVisitedElement(parent))
    {
        const auto parentResult = traverseElements(parent, traversedElements, actionFn);
        if (parentResult)
        {
            return parentResult;
        }
    }
    return std::nullopt;
}

static std::shared_ptr<ChainLinkElement>
getExtremityElementFromRoot(const std::shared_ptr<ChainLinkElement>& rootElement)
{
    auto auxElement = rootElement;
    while (!auxElement->getLinksTo().empty()) { auxElement = auxElement->getLinksTo().front(); }
    return auxElement;
}

static bool doesChainContainTile(const std::shared_ptr<ChainLinkElement>& rootElement,
                                 const std::shared_ptr<SolverTile>& tile)
{
    std::unordered_set<std::shared_ptr<ChainLinkElement>> traversedElements;
    const auto result =
        traverseElements(rootElement,
                         traversedElements,
                         [&tile](const std::shared_ptr<ChainLinkElement>& element) -> bool {
                             return tile == element->getSourceTile();
                         });

    return result.has_value();
}

template<typename A, typename B>
std::pair<B, A> flip_pair(const std::pair<A, B>& p)
{
    return std::pair<B, A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B, A> flip_map(const std::unordered_map<A, B>& src)
{
    std::multimap<B, A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), flip_pair<A, B>);
    return dst;
}

class SinglesChainsAnalyzer
{
  public:
    SinglesChainsAnalyzer(Chain& chain, Solver& solver)
        : m_chain(chain)
        , m_solver(solver)
    {}

    bool analyzeAndPerform()
    {
        auto result = checkTwiceInRegion();
        if (result)
            return true;
        result = checkTwoColorsElsewhere();
        return result;
    }

  private:
    bool checkTwiceInRegion()
    {
        for (const auto& rootElement : m_chain.getRootElements())
        {
            const std::shared_ptr<ChainLinkElement> extremityElement =
                getExtremityElementFromRoot(rootElement);

            std::unordered_set<std::shared_ptr<ChainLinkElement>> traversedElements;

            const auto lookForTwiceLambda = [](const std::shared_ptr<ChainLinkElement>& element) {
                std::unordered_set<std::shared_ptr<ChainLinkElement>> visitedElements;
                return lookForTwiceInRegion(element, element, false, visitedElements);
            };

            const auto result =
                traverseElements(extremityElement, traversedElements, lookForTwiceLambda);
            if (result)
            {
                return true;
            }
        }
        return false;
    }

    bool checkTwoColorsElsewhere()
    {
        const auto chainedValue = m_chain.getTargetChainValue();
        for (const auto& rootElement : m_chain.getRootElements())
        {
            const std::shared_ptr<ChainLinkElement> extremityElement =
                getExtremityElementFromRoot(rootElement);
            for (const auto& tile : m_solver.getGridTiles())
            {
                const auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
                if (!solverTile->hasSuggestion(chainedValue))
                    continue;

                if (doesChainContainTile(rootElement, solverTile))
                    continue;

                // find all linked elements who can see the solverTile

                const auto checkIfSameRegionLambda =
                    [&solverTile, chainedValue](const std::shared_ptr<ChainLinkElement>& element) {
                        const auto elementSolverTile = element->getSourceTile();
                        assert(elementSolverTile->hasSuggestion(chainedValue));
                        return SolverUtils::areTilesInTheSameRegion(*elementSolverTile,
                                                                    *solverTile);
                    };

                std::unordered_set<std::shared_ptr<ChainLinkElement>> traversedElements;
                std::vector<std::shared_ptr<ChainLinkElement>> seeingElements;

                FoundElement hasFound;
                do {
                    const auto startPosition = hasFound.value_or(extremityElement);
                    hasFound =
                        traverseElements(startPosition, traversedElements, checkIfSameRegionLambda);
                    if (hasFound)
                    {
                        seeingElements.emplace_back(hasFound.value());
                    }
                } while (hasFound);

                if (seeingElements.size() < 2)
                    continue;

                const auto referenceLinkState = seeingElements.front()->getLinkState();
                assert(referenceLinkState != ElementLinkState::NONE);
                bool areStatesHomogeneous = true;
                for (const auto& element : seeingElements)
                {
                    areStatesHomogeneous = element->getLinkState() == referenceLinkState;
                    if (!areStatesHomogeneous)
                        break;
                }

                if (areStatesHomogeneous)
                    continue;

                solverTile->removeSuggestion(chainedValue);
                m_solver.report("Singles Chains - Two Colors elsewhere\nO tile {} pode ver duas "
                                "cores diferentes na chain criada para a sugestão {}. Dessa forma, "
                                "a sugestão {} pode ser retirada do tile.",
                                *solverTile,
                                chainedValue,
                                chainedValue);

                return true;
                // traverseElements(rootElement);
            }
        }
        return false;
    }

    using PairInSameRegion = std::optional<
        std::pair<std::shared_ptr<ChainLinkElement>, std::shared_ptr<ChainLinkElement>>>;

    static PairInSameRegion
    lookForTwiceInRegion(const std::shared_ptr<ChainLinkElement>& sourceElement,
                         const std::shared_ptr<ChainLinkElement>& iteratingElement,
                         bool shouldSkipLayer,
                         std::unordered_set<std::shared_ptr<ChainLinkElement>>& visitedElements)
    {
        visitedElements.emplace(iteratingElement);

        if (!shouldSkipLayer && sourceElement != iteratingElement)
        {
            const bool sameRegion = SolverUtils::areTilesInTheSameRegion(
                *sourceElement->getSourceTile(), *iteratingElement->getSourceTile());
            if (sameRegion)
            {
                assert(sourceElement->getLinkState() == iteratingElement->getLinkState());
                return std::make_pair(sourceElement, iteratingElement);
            }
        }

        // first, check for all children of current node
        for (const auto& child : iteratingElement->getLinksTo())
        {
            if (visitedElements.find(child) != visitedElements.cend())
            {
                continue;
            }

            const auto result =
                lookForTwiceInRegion(sourceElement, child, !shouldSkipLayer, visitedElements);
            if (result)
            {
                return result;
            }
        }

        // then, check for the ancestor
        const auto parent = iteratingElement->getLinkedFrom().lock();
        if (parent && parent != sourceElement &&
            visitedElements.find(parent) == visitedElements.cend())
        {
            const auto result =
                lookForTwiceInRegion(sourceElement, parent, !shouldSkipLayer, visitedElements);
            if (result)
            {
                return result;
            }
        }
        return std::nullopt;
    }

  private:
    Chain& m_chain;
    Solver& m_solver;
};

} // namespace

bool SinglesChains::analyze()
{
    std::unordered_map<TileValueType, int> suggestionsPerValue;
    for (const auto& region : m_solver.getAllRegions())
    {
        const auto& suggestionsQuan = region->getSuggestionsQuan();
        const auto& suggestionsWith2Appearances =
            suggestionsQuan.getSuggestionsWithQuantityEqualTo(2);
        for (const auto& suggestion : suggestionsWith2Appearances)
        {
            ++suggestionsPerValue[suggestion];
            if (suggestionsPerValue[suggestion] >= 3)
            {
                return true;
            }
        }
    }
    return false;
}

bool SinglesChains::perform()
{
    std::unordered_map<TileValueType, int> suggestionsPerValue;
    for (const auto& region : m_solver.getAllRegions())
    {
        const auto& suggestionsQuan = region->getSuggestionsQuan();
        const auto& suggestionsWith2Appearances =
            suggestionsQuan.getSuggestionsWithQuantityEqualTo(2);
        for (const auto& suggestion : suggestionsWith2Appearances)
        {
            ++suggestionsPerValue[suggestion];
        }
    }

    const auto valuePerSuggestions = flip_map(suggestionsPerValue);
    for (const auto& valueSuggestions : valuePerSuggestions)
    {
        if (valueSuggestions.first < 2)
        {
            continue;
        }
        const auto targetSuggestion = valueSuggestions.second;
        Chain chain(m_solver, targetSuggestion);
        chain.buildChain();
        SinglesChainsAnalyzer SCsAnalyzer(chain, m_solver);
        const auto result = SCsAnalyzer.analyzeAndPerform();
        if (result)
            return result;
    }

    return false;
}
