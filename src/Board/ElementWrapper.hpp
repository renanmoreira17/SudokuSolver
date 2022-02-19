#ifndef __ELEMENTWRAPPER_H__
#define __ELEMENTWRAPPER_H__

class Tile;

template<typename T>
class ElementWrapper
{
  private:
    T* m_element;

    ElementWrapper* m_next{nullptr};
    ElementWrapper* m_previous{nullptr};

  public:
    ElementWrapper(T* const& element)
        : m_element(element)
        , m_next(nullptr)
        , m_previous(nullptr)
    {}

    ElementWrapper(T* const& element, ElementWrapper* const& previous)
        : m_element(element)
        , m_next(nullptr)
        , m_previous(previous)
    {
        if (previous)
            previous->m_next = this;
    }
    ~ElementWrapper() {}

    ElementWrapper(const ElementWrapper& other) = delete;
    ElementWrapper(ElementWrapper&& other)
    {
        m_element = other.m_element;
        m_next = other.m_next;
        m_previous = other.m_previous;

        other.m_next = nullptr;
        other.m_previous = nullptr;
        other.m_element = nullptr;

        if (m_next)
            m_next->m_previous = this;
        if (m_previous)
            m_previous->m_next = this;
    }
    ElementWrapper& operator=(const ElementWrapper& other) = delete;

    ElementWrapper& operator=(ElementWrapper&& other)
    {
        if (&other == this)
            return *this;

        m_element = other.m_element;
        m_next = other.m_next;
        m_previous = other.m_previous;

        other.m_next = nullptr;
        other.m_previous = nullptr;
        other.m_element = nullptr;

        if (m_next)
            m_next->m_previous = this;
        if (m_previous)
            m_previous->m_next = this;

        return *this;
    }

    ElementWrapper* const& next() const { return m_next; }

    ElementWrapper* const& previous() const { return m_previous; }

    bool hasNext() const { return m_next; }
    bool hasPrevious() const { return m_previous; }

    T* const& getElementPtr() const { return m_element; }
    T& getElement() { return *m_element; };
    const T& getElement() const { return *m_element; }
};

template<typename T>
class LineElementWrapper : public ElementWrapper<T>
{
  private:
  public:
    LineElementWrapper(T* const& element)
        : ElementWrapper<T>(element)
    {}
    LineElementWrapper(T* const& element, LineElementWrapper<T>* const& previous)
        : ElementWrapper<T>(element, previous)
    {}
    ~LineElementWrapper() {}
};

using LineTileWrapper = LineElementWrapper<Tile>;

template<typename T>
class SubgridElementWrapper : public ElementWrapper<T>
{
  private:
    /* data */
  public:
    SubgridElementWrapper(T* const& element)
        : ElementWrapper<T>(element)
    {}
    SubgridElementWrapper(T* const& element, SubgridElementWrapper<T>* const& previous)
        : ElementWrapper<T>(element, previous)
    {}
    ~SubgridElementWrapper() {}

    const ElementWrapper<T>* up() const;
    const ElementWrapper<T>* down() const;
};

using SubgridTileWrapper = SubgridElementWrapper<Tile>;

#endif // __ELEMENTWRAPPER_H__
