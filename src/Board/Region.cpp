#include "Region.hpp"

Grid* Region::getGrid() const
{
    return m_grid;
}

Region::Region(const short index, const RegionType regionType, Grid* grid)
    : m_index(index)
    , m_type(regionType)
    , m_grid(grid)
{}

const ElementList<std::shared_ptr<Tile>>& Region::getTiles() const
{
    return *m_elementList;
}

void Region::setElementList(const std::shared_ptr<ElementList<std::shared_ptr<Tile>>>& elementList)
{
    m_elementList = elementList;
}

short Region::getIndex() const
{
    return m_index;
}
RegionType Region::getType() const
{
    return m_type;
}

bool Region::hasValue(const TileValueType value) const
{
    const auto first = m_elementList->cbegin();
    const auto last = m_elementList->cend();

    const auto found =
        std::find_if(first, last, [&](const std::shared_ptr<Tile>& ele) { return *ele == value; });
    return found != last;
}

bool Region::isCompleted() const
{
    for (auto&& tile : *this)
    {
        if (!tile->hasValue())
        {
            return false;
        }
    }
    return true;
}

typename ElementList<std::shared_ptr<Tile>>::iterator Region::begin()
{
    return m_elementList->begin();
}
typename ElementList<std::shared_ptr<Tile>>::iterator Region::end()
{
    return m_elementList->end();
}
typename ElementList<std::shared_ptr<Tile>>::const_iterator Region::cbegin() const
{
    return m_elementList->cbegin();
}
typename ElementList<std::shared_ptr<Tile>>::const_iterator Region::cend() const
{
    return m_elementList->cend();
}
typename ElementList<std::shared_ptr<Tile>>::const_iterator Region::begin() const
{
    return cbegin();
}
typename ElementList<std::shared_ptr<Tile>>::const_iterator Region::end() const
{
    return cend();
}

static std::shared_ptr<ElementList<std::shared_ptr<Tile>>>
Region::make_line(LineOrientation orientation,
                  TileValueType index,
                  GridTiles& gridElements,
                  const std::optional<const std::function<void(const std::shared_ptr<Tile>&)>>& applyElement)
{
    LineElementWrapper<std::shared_ptr<Tile>>* lastElementWrapper = nullptr;
    for (TileValueType i = 0; i < 9; i++)
    {
        const TileValueType col = orientation == LineOrientation::VERTICAL ? index : i;
        const TileValueType row = orientation == LineOrientation::HORIZONTAL ? index : i;

        std::shared_ptr<Tile>& element = gridElements(row, col);

        if (applyElement)
            applyElement.value()(element);

        auto elementWrapper = new LineElementWrapper<std::shared_ptr<Tile>>(&element, lastElementWrapper);
        lastElementWrapper = elementWrapper;
    }
    return std::make_shared<ElementList<std::shared_ptr<Tile>>>(lastElementWrapper);
}

static std::shared_ptr<ElementList<std::shared_ptr<Tile>>> Region::make_subgrid(
    TileValueType index,
    GridTiles& gridElements,
    const std::optional<const std::function<void(const std::shared_ptr<Tile>&)>>& applyElement)
{
    SubgridElementWrapper<std::shared_ptr<Tile>>* lastElementWrapper = nullptr;

    const TileValueType offsetCol = index % 3;
    const TileValueType offsetRow = index / 3;

    for (TileValueType i = 0; i < 9; i++)
    {
        const TileValueType col = offsetCol * 3 + i % 3;
        const TileValueType row = offsetRow * 3 + i / 3;

        auto& element = gridElements(row, col);

        if (applyElement)
            applyElement.value()(element);

        auto tileWrapper = new SubgridElementWrapper<std::shared_ptr<Tile>>(&element, lastElementWrapper);
        lastElementWrapper = tileWrapper;
    }
    return std::make_shared<ElementList<std::shared_ptr<Tile>>>(lastElementWrapper);
}
