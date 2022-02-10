#ifndef __TRAVERSAL_H__
#define __TRAVERSAL_H__

#include "Solver/SolverTypes.hpp"

#include <memory>
#include <unordered_set>

class ChainLinkElement;
class SolverTile;

class Traversal
{
  public:
    Traversal(const std::shared_ptr<ChainLinkElement>& rootElement,
              const std::function<bool(const std::shared_ptr<ChainLinkElement>&)>& actionFn);

    using FoundElement = std::optional<std::shared_ptr<ChainLinkElement>>;

    FoundElement traverse();

    FoundElement continueTraversing(const std::shared_ptr<ChainLinkElement>& fromElement);

    static std::shared_ptr<ChainLinkElement>
    getExtremityElementFrom(const std::shared_ptr<ChainLinkElement>& rootElement);

    static bool doesChainContainTile(const std::shared_ptr<ChainLinkElement>& rootElement,
                                     const SolverTilePtr& tile);

  private:
    FoundElement internalTraverse(const std::shared_ptr<ChainLinkElement>& element);

    const std::shared_ptr<ChainLinkElement> m_rootElement;
    std::unordered_set<std::shared_ptr<ChainLinkElement>> m_traversedElements;
    const std::function<bool(const std::shared_ptr<ChainLinkElement>&)> m_actionFn;
};

#endif // __TRAVERSAL_H__
