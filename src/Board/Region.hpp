#ifndef __REGION_H__
#define __REGION_H__

#include "ElementList.hpp"
#include "ElementsContainer.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <set>
#include <memory>

enum Orientation {vertical, horizontal};

template<typename T>
class Region
{
protected:
    std::shared_ptr<ElementList<T>> m_elementList;
    TileValueType m_index;
public:
    Region() = default;
    Region(TileValueType index) : m_index(index) {}
    Region(const Region<T>& other) = default;
    Region(Region<T>&& other) = default;
    Region<T>& operator=(const Region& other) = default;
    Region<T>& operator=(Region&& other) = default;
    virtual ~Region() = default;

    std::shared_ptr<ElementList<T>> getElementList() const { return m_elementList; };

    TileValueType getIndex() const { return m_index; }

    bool hasValue(const TileValueType value) const
    {
        auto first = m_elementList->cbegin();
        auto last = m_elementList->cend();

        auto found = std::find(first, last, value);
        return found != last;
    }

    bool isCompleted() const
    {
        for (auto &&element : *this)
        {
            auto& tile = getTileFromT(element);
            if (!tile.hasValue())
            {
                return false;
            }
        }
        return true;
    }

    const Tile& getTileFromT(const T& element) const;

    typename ElementList<T>::iterator begin()
    {
        return m_elementList->begin();
    }
    typename ElementList<T>::iterator end()
    {
        return m_elementList->end();
    }
    typename ElementList<T>::const_iterator cbegin() const
    {
        return m_elementList->cbegin();
    }
    typename ElementList<T>::const_iterator cend() const
    {
        return m_elementList->cend();
    }
    typename ElementList<T>::const_iterator begin() const
    {
        return cbegin();
    }
    typename ElementList<T>::const_iterator end() const
    {
        return cend();
    }


    static std::shared_ptr<ElementList<T>> make_line(const Orientation& orientation, TileValueType index, GridElements<T>& gridElements, const std::optional<const std::function<void(T&)>>& applyElement)  {
        LineElementWrapper<T>* lastElementWrapper = nullptr;
        for (TileValueType i = 0; i < 9; i++)
        {
            const TileValueType col = orientation == vertical ? index : i;
            const TileValueType row = orientation == horizontal ? index : i;
            T& element = gridElements(row, col);
            if(applyElement.has_value())
                applyElement.value()(element);
            auto elementWrapper = new LineElementWrapper<T>(&element, lastElementWrapper);
            lastElementWrapper = elementWrapper;
        }
        return std::make_shared<ElementList<T>>(lastElementWrapper);
    }

    static std::shared_ptr<ElementList<T>> make_subgrid(TileValueType index, GridElements<T>& gridElements, const std::optional<const std::function<void(T&)>>& applyElement)  {
        SubgridElementWrapper<T>* lastElementWrapper = nullptr;
        const TileValueType offsetCol = index % 3;
        const TileValueType offsetRow = index / 3;
        for (TileValueType i = 0; i < 9; i++)
        {
            const TileValueType col = offsetCol*3 + i%3;
            const TileValueType row = offsetRow*3 + i/3;
            auto& element = gridElements(row, col);
            if(applyElement.has_value())
                applyElement.value()(element);
            auto tileWrapper = new SubgridElementWrapper<T>(&element, lastElementWrapper);
            lastElementWrapper = tileWrapper;
        }
        return std::make_shared<ElementList<T>>(lastElementWrapper);
    }
};

#endif // __REGION_H__
