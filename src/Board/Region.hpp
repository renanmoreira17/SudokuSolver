#ifndef __REGION_H__
#define __REGION_H__

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <set>

#include "ElementList.hpp"
#include "ElementsContainer.hpp"
#include "Util/GlobalDefinitions.hpp"

class Region
{
  public:
    enum class RegionType
    {
        LINE,
        SUBGRID
    };

    virtual RegionSpecificType getRegionSpecificType() const = 0;

  private:
    Grid* m_grid;

  protected:
    std::shared_ptr<ElementList<std::shared_ptr<Tile>>> m_elementList;
    short m_index;
    RegionType m_type;

  public:
    Region(const short index, const RegionType regionType, Grid* grid);
    Region(const Region& other) = default;
    Region(Region&& other) = default;
    Region& operator=(const Region& other) = default;
    Region& operator=(Region&& other) = default;
    virtual ~Region() = default;

    void setElementList(const std::shared_ptr<ElementList<std::shared_ptr<Tile>>>& elementList);

    const ElementList<std::shared_ptr<Tile>>& getTiles() const;

    short getIndex() const;
    RegionType getType() const;

    Grid* getGrid() const;

    bool hasValue(const TileValueType value) const;

    bool isCompleted() const;

    typename ElementList<std::shared_ptr<Tile>>::iterator begin();
    typename ElementList<std::shared_ptr<Tile>>::iterator end();
    typename ElementList<std::shared_ptr<Tile>>::const_iterator cbegin() const;
    typename ElementList<std::shared_ptr<Tile>>::const_iterator cend() const;
    typename ElementList<std::shared_ptr<Tile>>::const_iterator begin() const;
    typename ElementList<std::shared_ptr<Tile>>::const_iterator end() const;

    static std::shared_ptr<ElementList<std::shared_ptr<Tile>>>
    make_line(LineOrientation orientation,
              TileValueType index,
              GridTiles& gridElements,
              const std::optional<const std::function<void(const std::shared_ptr<Tile>&)>>& applyElement);

    static std::shared_ptr<ElementList<std::shared_ptr<Tile>>>
    make_subgrid(TileValueType index,
                 GridTiles& gridElements,
                 const std::optional<const std::function<void(const std::shared_ptr<Tile>&)>>& applyElement);
};

#endif // __REGION_H__
