#ifndef __CHAIN_H__
#define __CHAIN_H__

#include "Solver/SolverTypes.hpp"

#include <memory>
#include <optional>
#include <vector>

class SolverTile;

class ChainLinkElement : public std::enable_shared_from_this<ChainLinkElement>
{
  protected:
    ChainLinkElement(const SolverTilePtr& sourceTile);

  public:
    ChainLinkElement() = delete;
    virtual ~ChainLinkElement() = default;

    static std::shared_ptr<ChainLinkElement> create(const SolverTilePtr& sourceTile)
    {
        return std::shared_ptr<ChainLinkElement>(new ChainLinkElement(sourceTile));
    }

    const SolverTilePtr& getSourceTile() const;
    const std::vector<std::shared_ptr<ChainLinkElement>>& getLinksTo() const;
    const std::weak_ptr<ChainLinkElement>& getLinkedFrom() const;

    virtual void linkTo(const std::shared_ptr<ChainLinkElement>& targetTile);

  private:
    SolverTilePtr m_sourceTile;
    std::vector<std::shared_ptr<ChainLinkElement>> m_linksTo;
    std::weak_ptr<ChainLinkElement> m_linkedFrom;
};

class Chain
{
  public:
    Chain() = default;

    void buildChainForElement(const SolverTilePtr& tile);

    const std::vector<std::shared_ptr<ChainLinkElement>>& getRootElements() const;

  protected:
    virtual std::shared_ptr<ChainLinkElement> createRootElement(const SolverTilePtr& tile) = 0;

    virtual bool shouldLinkToElement(const std::shared_ptr<ChainLinkElement>& element,
                                     const std::shared_ptr<ChainLinkElement>& origin) const = 0;

    virtual std::vector<std::shared_ptr<ChainLinkElement>>
    getLinksFromElement(const std::shared_ptr<ChainLinkElement>& element,
                        const std::optional<std::shared_ptr<ChainLinkElement>>& origin) = 0;

  private:
    void findLinksForElement(const std::shared_ptr<ChainLinkElement>& element,
                             const std::optional<std::shared_ptr<ChainLinkElement>>& origin);

    std::vector<std::shared_ptr<ChainLinkElement>> m_rootElements;
};

#endif // __CHAIN_H__
