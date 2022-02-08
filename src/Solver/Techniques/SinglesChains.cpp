#include "SinglesChains.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverUtils.hpp"
#include "Tools/Chain.hpp"
#include "Tools/Traversal.hpp"

#include <map>
#include <memory>
#include <optional>
#include <unordered_set>
#include <cassert>

#include "Util/UtilFunctions.hpp"
#include <fmt/format.h>
#include <iostream>

#ifdef DEBUG
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

/*
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
*/

class SinglesChainLinkElement : public ChainLinkElement
{
  protected:
    SinglesChainLinkElement(const std::shared_ptr<SolverTile>& sourceTile)
        : ChainLinkElement(sourceTile)
    {}

  public:
    SinglesChainLinkElement() = delete;

    static std::shared_ptr<SinglesChainLinkElement>
    create(const std::shared_ptr<SolverTile>& sourceTile)
    {
        return std::shared_ptr<SinglesChainLinkElement>(new SinglesChainLinkElement(sourceTile));
    }

    RegionSpecificType getLinkType() const { return m_linkType; }
    void setLinkType(RegionSpecificType type) { m_linkType = type; }
    ElementLinkState getLinkState() const { return m_linkState; }
    void setLinkState(const ElementLinkState state) { m_linkState = state; }

    void linkTo(const std::shared_ptr<ChainLinkElement>& targetTile) override
    {
        ChainLinkElement::linkTo(targetTile);
        std::dynamic_pointer_cast<SinglesChainLinkElement>(targetTile)
            ->setLinkState(!getLinkState());
    }

  private:
    RegionSpecificType m_linkType{RegionSpecificType::UNKNOWN};
    ElementLinkState m_linkState{ElementLinkState::NONE};
};

class SinglesChain : public Chain
{
  public:
    SinglesChain(Solver& solver, const TileValueType targetChainValue)
        : m_solver(solver)
        , m_targetChainValue(targetChainValue)
    {}
    SinglesChain() = delete;

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

            buildChainForElement(rootTile);
        }
#ifdef DEBUG
        printChain();
#endif
    }

    std::shared_ptr<ChainLinkElement>
    createRootElement(const std::shared_ptr<SolverTile>& tile) override
    {
        m_visitedTiles.clear();
        const auto element = SinglesChainLinkElement::create(tile);
        element->setLinkState(ElementLinkState::RED);
        return element;
    }

  protected:
    virtual std::vector<std::shared_ptr<ChainLinkElement>>
    getLinksFromElement(const std::shared_ptr<ChainLinkElement>& element,
                        const std::optional<std::shared_ptr<ChainLinkElement>>& origin) override
    {
        const auto& sourceTile = element->getSourceTile();
        const auto insertedInfo = m_visitedTiles.insert(sourceTile);
        if (!insertedInfo.second)
        {
            return {};
        }

        std::vector<std::shared_ptr<ChainLinkElement>> result;
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
            std::erase_if(tiles, [&](const std::shared_ptr<SolverTile>& tile) {
                return tile == sourceTile ||
                       (origin.has_value() && tile == origin.value()->getSourceTile()) ||
                       hasVisitedTile(tile);
            });
            if (tiles.empty())
            {
                continue;
            }

            assert(tiles.size() == 1);

            const auto targetElement = SinglesChainLinkElement::create(tiles.front());
            targetElement->setLinkType(region->getRegionSpecificType());
            result.push_back(targetElement);
        }

        return result;
    }

    bool shouldLinkToElement(const std::shared_ptr<ChainLinkElement>& element,
                             const std::shared_ptr<ChainLinkElement>&) const override
    {
        return !hasVisitedTile(element->getSourceTile());
    }

    bool hasVisitedTile(const std::shared_ptr<SolverTile>& tile) const
    {
        return m_visitedTiles.find(tile) != m_visitedTiles.cend();
    }

  public:
    TileValueType getTargetChainValue() const { return m_targetChainValue; }

  private:
    Solver& m_solver;
    const TileValueType m_targetChainValue;
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
            std::function<void(const std::shared_ptr<SinglesChainLinkElement>&, int)>
                printLinksFor = [&](const std::shared_ptr<SinglesChainLinkElement>& element,
                                    int depth = 0) {
                    const auto& sourceTileCoord = element->getSourceTile()->getCoordinates();
                    const auto& links = element->getLinksTo();
                    // if (!links.empty())
                    // {
                    //     fmt::print("Entering depth {}\n", depth);
                    // }
                    for (const auto& linkedTo : links)
                    {
                        const auto singlesChainLinkedTo =
                            std::dynamic_pointer_cast<SinglesChainLinkElement>(linkedTo);
                        const auto& targetTileCoord = linkedTo->getSourceTile()->getCoordinates();
                        const std::string depthStr = std::string(depth * 2, ' ');
                        fmt::print(
                            "{}{}{}({}) -> {}{}({}) (Linked through: {})\n",
                            depthStr,
                            convertRowToLetter(sourceTileCoord.row),
                            sourceTileCoord.col + 1,
                            convertElementLinkStateToString(element->getLinkState()),
                            convertRowToLetter(targetTileCoord.row),
                            targetTileCoord.col + 1,
                            convertElementLinkStateToString(singlesChainLinkedTo->getLinkState()),
                            convertRegionSpecificTypeToString(singlesChainLinkedTo->getLinkType()));
                        printLinksFor(singlesChainLinkedTo, depth + 1);
                    }
                    // if (!links.empty())
                    // {
                    //     fmt::print("Exiting depth {}\n", depth);
                    // }
                };
            printLinksFor(std::dynamic_pointer_cast<SinglesChainLinkElement>(rootElement), 0);
            fmt::print("\n");
        }
    }
#endif // DEBUG
};

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
    SinglesChainsAnalyzer(SinglesChain& chain, Solver& solver)
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
            std::shared_ptr<ChainLinkElement> foundTwiceInRegionElement;

            const auto lookForTwiceLambda =
                [&foundTwiceInRegionElement](const std::shared_ptr<ChainLinkElement>& element) {
                    const auto elementSingles =
                        std::dynamic_pointer_cast<SinglesChainLinkElement>(element);

                    Traversal twiceInRegionTraversal(
                        element,
                        [&elementSingles](const std::shared_ptr<ChainLinkElement>& elementReg) {
                            const auto elementRegSingles =
                                std::dynamic_pointer_cast<SinglesChainLinkElement>(elementReg);
                            if (elementSingles != elementRegSingles &&
                                elementSingles->getLinkState() == elementRegSingles->getLinkState())
                            {
                                const bool sameRegion = SolverUtils::areTilesInTheSameRegion(
                                    *elementSingles->getSourceTile(),
                                    *elementRegSingles->getSourceTile());
                                if (sameRegion)
                                {
                                    return true;
                                }
                            }
                            return false;
                        });
                    const auto result = twiceInRegionTraversal.traverse();
                    if (result)
                    {
                        foundTwiceInRegionElement = result.value();
                        return true;
                    }
                    return false;
                };

            Traversal twiceInLambdaTraversal(rootElement, lookForTwiceLambda);
            const auto result = twiceInLambdaTraversal.traverse();
            if (result)
            {
                const auto elementSingles =
                    std::dynamic_pointer_cast<SinglesChainLinkElement>(result.value());
                const auto elementSinglesReg =
                    std::dynamic_pointer_cast<SinglesChainLinkElement>(foundTwiceInRegionElement);
                // treat twice in region
                m_solver.report(
                    "\n\nTwice in region = {} {} from {} and {} {} from {} equal = {}\n\n",
                    *elementSingles->getSourceTile(),
                    convertRegionSpecificTypeToString(elementSingles->getLinkType()),
                    *elementSingles->getLinkedFrom().lock()->getSourceTile(),
                    *elementSinglesReg->getSourceTile(),
                    convertRegionSpecificTypeToString(elementSinglesReg->getLinkType()),
                    *elementSinglesReg->getLinkedFrom().lock()->getSourceTile(),
                    elementSingles == elementSinglesReg);
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
                Traversal::getExtremityElementFrom(rootElement);
            for (const auto& tile : m_solver.getGridTiles())
            {
                const auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
                if (!solverTile->hasSuggestion(chainedValue))
                    continue;

                if (Traversal::doesChainContainTile(rootElement, solverTile))
                    continue;

                // find all linked elements who can see the solverTile

                const auto checkIfSameRegionLambda =
                    [&solverTile, chainedValue](const std::shared_ptr<ChainLinkElement>& element) {
                        const auto elementSolverTile = element->getSourceTile();
                        assert(elementSolverTile->hasSuggestion(chainedValue));
                        return SolverUtils::areTilesInTheSameRegion(*elementSolverTile,
                                                                    *solverTile);
                    };

                std::vector<std::shared_ptr<ChainLinkElement>> seeingElements;

                Traversal seeingTraversal(extremityElement, checkIfSameRegionLambda);
                auto hasFound = seeingTraversal.traverse();
                while (hasFound)
                {
                    const auto& foundValue = hasFound.value();
                    seeingElements.push_back(foundValue);
                    hasFound = seeingTraversal.continueTraversing(foundValue);
                }

                if (seeingElements.size() < 2)
                    continue;

                const auto referenceLinkState =
                    std::dynamic_pointer_cast<SinglesChainLinkElement>(seeingElements.front())
                        ->getLinkState();
                assert(referenceLinkState != ElementLinkState::NONE);
                bool areStatesHomogeneous = true;
                for (const auto& element : seeingElements)
                {
                    areStatesHomogeneous =
                        std::dynamic_pointer_cast<SinglesChainLinkElement>(element)
                            ->getLinkState() == referenceLinkState;
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

  private:
    SinglesChain& m_chain;
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
        SinglesChain chain(m_solver, targetSuggestion);
        chain.buildChain();
        SinglesChainsAnalyzer SCsAnalyzer(chain, m_solver);
        const auto result = SCsAnalyzer.analyzeAndPerform();
        if (result)
            return result;
    }

    return false;
}
