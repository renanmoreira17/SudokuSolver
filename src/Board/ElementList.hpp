#ifndef __ELEMENTLIST_H__
#define __ELEMENTLIST_H__

// #include "Tile.hpp"
#include "ElementWrapper.hpp"
#include <iterator>

class Tile;

template<typename T>
class ElementList
{
private:
    ElementWrapper<T> *m_head;
    ElementWrapper<T> *m_tail;

public:
    ElementList(ElementWrapper<T> *ptr) {
        m_head = ptr;
        m_tail = ptr;
        while (m_tail->next())
        {
            m_tail = m_tail->next();
        }
        while (m_head->previous())
        {
            m_head = m_head->previous();
        }
    }
    ElementList(ElementWrapper<T> *const head, ElementWrapper<T> *const tail)
        : m_head(head), m_tail(tail)
    {
    }
    ~ElementList() {
        ElementWrapper<T>* ptr;
        while ((ptr = m_head))
        {
            m_head = m_head->next();
            delete ptr;
        }
    }

    ElementWrapper<T> *const &getHead() const { return m_head; }
    ElementWrapper<T> *const &getTail() const { return m_tail; }

    struct const_iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T *;
        using reference = const T &;

    protected:
        ElementWrapper<T> *m_ptr;

        void prot_incr()
        {
            m_ptr = m_ptr->next();
        }
        void prot_decr() {
            m_ptr = m_ptr->previous();
        }

    public:
        const_iterator(ElementWrapper<T> *ptr) : m_ptr(ptr){};

        reference operator*() const
        {
            return m_ptr->getElement();
        }

        pointer operator->() const
        {
            return pointer(&m_ptr->getElement());
        }

        //Increment / Decrement
        const_iterator &operator++()
        {
            prot_incr();
            return *this;
        }

        const_iterator operator++(int)
        {
            ElementWrapper<T> *tmp = m_ptr;
            ++*this;
            return const_iterator(tmp);
        }

        const_iterator &operator--()
        {
            prot_decr();
            return *this;
        }

        const_iterator operator--(int)
        {
            ElementWrapper<T> *tmp = m_ptr;
            --*this;
            return const_iterator(tmp);
        }

        //Comparison operators
        bool operator==(const const_iterator &r) const
        {
            return m_ptr == r.m_ptr;
        }

        bool operator!=(const const_iterator &r) const
        {
            return m_ptr != r.m_ptr;
        }
    };

    struct iterator : public ElementList::const_iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

        iterator(ElementWrapper<T> *ptr) : const_iterator(ptr) {}

        //Pointer like operators
        reference operator*() const
        {
            return this->m_ptr->getElement();
        }
        pointer operator->() const
        {
            return pointer(&this->m_ptr->getElement());
        }

        //Increment / Decrement
        iterator &operator++()
        {
            this->prot_incr();
            return *this;
        }

        iterator operator++(int)
        {
            ElementWrapper<T>* tmp = this->m_ptr;
            ++*this;
            return iterator(tmp);
        }

        iterator &operator--()
        {
            this->prot_decr();
            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
    };

    iterator begin()
    {
        return iterator(m_head);
    }
    iterator end()
    {
        return iterator(m_tail->next());
    }
    const_iterator cbegin() const
    {
        return const_iterator(m_head);
    }
    const_iterator cend() const
    {
        return const_iterator(m_tail->next());
    }
    const_iterator begin() const
    {
        return cbegin();
    }
    const_iterator end() const
    {
        return cend();
    }
};

using TileList = ElementList<Tile>;


#endif // __ELEMENTLIST_H__


