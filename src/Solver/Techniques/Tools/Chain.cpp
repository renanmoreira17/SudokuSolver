#include "Chain.hpp"

#include <cassert>

ChainLinkElement::ChainLinkElement(const std::shared_ptr<SolverTile>& sourceTile)
    : m_sourceTile(sourceTile)
{}

const std::shared_ptr<SolverTile>& ChainLinkElement::getSourceTile() const
{
    return m_sourceTile;
}
const std::vector<std::shared_ptr<ChainLinkElement>>& ChainLinkElement::getLinksTo() const
{
    return m_linksTo;
}
const std::weak_ptr<ChainLinkElement>& ChainLinkElement::getLinkedFrom() const
{
    return m_linkedFrom;
}

void ChainLinkElement::linkTo(const std::shared_ptr<ChainLinkElement>& targetTile)
{
    m_linksTo.emplace_back(targetTile);
    assert(targetTile->m_linkedFrom.expired());
    targetTile->m_linkedFrom = weak_from_this();
}

const std::vector<std::shared_ptr<ChainLinkElement>>& Chain::getRootElements() const
{
    return m_rootElements;
}

void Chain::buildChainForElement(const std::shared_ptr<SolverTile>& tile)
{
    const auto currentRootElement = createRootElement(tile);

    findLinksForElement(currentRootElement, std::nullopt);
    m_rootElements.emplace_back(currentRootElement);
}

void Chain::findLinksForElement(const std::shared_ptr<ChainLinkElement>& element,
                                const std::optional<std::shared_ptr<ChainLinkElement>>& origin)
{
    const auto links = getLinksFromElement(element, origin);

    for (const auto& link : links)
    {
        if (!shouldLinkToElement(link, element))
        {
            continue;
        }
        element->linkTo(link);
        findLinksForElement(link, /*origin=*/element);
    }
}
