#include "Traversal.hpp"

#include "Chain.hpp"

Traversal::Traversal(const std::shared_ptr<ChainLinkElement>& rootElement,
                     const std::function<bool(const std::shared_ptr<ChainLinkElement>&)>& actionFn)
    : m_rootElement(rootElement)
    , m_actionFn(actionFn)
{}

Traversal::FoundElement Traversal::traverse()
{
    m_traversedElements.clear();
    return internalTraverse(m_rootElement);
}

Traversal::FoundElement
Traversal::continueTraversing(const std::shared_ptr<ChainLinkElement>& fromElement)
{
    return internalTraverse(fromElement);
}

Traversal::FoundElement
Traversal::internalTraverse(const std::shared_ptr<ChainLinkElement>& element)
{
    const auto hasVisitedElement = [this](const std::shared_ptr<ChainLinkElement>& element) {
        return m_traversedElements.find(element) != m_traversedElements.cend();
    };

    if (!hasVisitedElement(element))
    {
        m_traversedElements.emplace(element);
        const auto result = m_actionFn(element);
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
        const auto childResult = internalTraverse(child);
        if (childResult)
        {
            return childResult;
        }
    }
    const auto parent = element->getLinkedFrom().lock();
    if (parent && !hasVisitedElement(parent))
    {
        const auto parentResult = internalTraverse(parent);
        if (parentResult)
        {
            return parentResult;
        }
    }
    return std::nullopt;
}

std::shared_ptr<ChainLinkElement>
Traversal::getExtremityElementFrom(const std::shared_ptr<ChainLinkElement>& rootElement)
{
    auto auxElement = rootElement;
    while (!auxElement->getLinksTo().empty()) { auxElement = auxElement->getLinksTo().front(); }
    return auxElement;
}

bool Traversal::doesChainContainTile(const std::shared_ptr<ChainLinkElement>& rootElement,
                                     const std::shared_ptr<SolverTile>& tile)
{
    Traversal findTraversal(rootElement,
                            [&tile](const std::shared_ptr<ChainLinkElement>& element) -> bool {
                                return tile == element->getSourceTile();
                            });
    const auto result = findTraversal.traverse();

    return result.has_value();
}
