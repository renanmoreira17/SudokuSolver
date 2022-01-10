#ifndef __GRIDELEMENTS_H__
#define __GRIDELEMENTS_H__

#include "Tile.hpp"
#include <array>
#include <iterator>

template<typename T, TileValueType R, TileValueType C>
class ElementsContainer
{
  private:
    std::array<T, C * R> m_elements;

  public:
    struct const_iterator
    {
      protected:
        T* m_ptr;

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::random_access_iterator_tag;

      public:
        const_iterator(T* ptr)
            : m_ptr(ptr)
        {}

        reference operator*() const { return *m_ptr; }

        pointer operator->() const { return m_ptr; }

        reference operator[](difference_type off) const { return m_ptr[off]; }

        const_iterator& operator++()
        {
            ++m_ptr;
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator& operator--()
        {
            --m_ptr;
            return *this;
        }

        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        const_iterator& operator+=(difference_type off)
        {
            this->m_ptr += off;
            return *this;
        }

        const_iterator operator+(difference_type off) const { return const_iterator(m_ptr + off); }

        friend const_iterator operator+(difference_type off, const const_iterator& right)
        {
            return const_iterator(off + right.m_ptr);
        }

        const_iterator& operator-=(difference_type off)
        {
            m_ptr -= off;
            return *this;
        }

        const_iterator operator-(difference_type off) const { return const_iterator(m_ptr - off); }

        difference_type operator-(const const_iterator& right) const { return m_ptr - right.m_ptr; }

        // Comparison operators
        bool operator==(const const_iterator& r) const { return m_ptr == r.m_ptr; }

        bool operator!=(const const_iterator& r) const { return m_ptr != r.m_ptr; }

        bool operator<(const const_iterator& r) const { return m_ptr < r.m_ptr; }

        bool operator<=(const const_iterator& r) const { return m_ptr <= r.m_ptr; }

        bool operator>(const const_iterator& r) const { return m_ptr > r.m_ptr; }

        bool operator>=(const const_iterator& r) const { return m_ptr >= r.m_ptr; }
    };

    struct iterator : public ElementsContainer::const_iterator
    {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        iterator(pointer ptr)
            : const_iterator(ptr)
        {}

        // Pointer like operators
        reference operator*() const { return *this->m_ptr; }

        value_type* operator->() const { return this->m_ptr; }

        reference operator[](difference_type off) const { return this->m_ptr[off]; }

        // Increment / Decrement
        iterator& operator++()
        {
            ++this->m_ptr;
            return *this;
        }

        iterator operator++(int)
        {
            pointer tmp = this->m_ptr;
            ++*this;
            return iterator(tmp);
        }

        iterator& operator--()
        {
            --this->m_ptr;
            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;
            return iterator(tmp);
        }

        // Arithmetic
        iterator& operator+=(difference_type off)
        {
            this->m_ptr += off;
            return *this;
        }

        iterator operator+(difference_type off) const { return iterator(this->m_ptr + off); }

        friend iterator operator+(difference_type off, const iterator& right)
        {
            return iterator(off + right.m_ptr);
        }

        iterator& operator-=(difference_type off)
        {
            this->m_ptr -= off;
            return *this;
        }

        iterator operator-(difference_type off) const { return iterator(this->m_ptr - off); }

        difference_type operator-(const const_iterator& right) const
        {
            return static_cast<const const_iterator&>(*this) - right;
        }
    };

    ElementsContainer() = default;
    ~ElementsContainer() = default;

    iterator begin() { return iterator(&m_elements[0]); }
    iterator end() { return iterator(&m_elements[R * C]); }
    const_iterator cbegin() const { return const_iterator((T*)&m_elements[0]); }
    const_iterator cend() const { return const_iterator((T*)&m_elements[R * C]); }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

    T& operator()(TileValueType row, TileValueType col) { return m_elements[row * R + col]; }
    const T& operator()(TileValueType row, TileValueType col) const
    {
        return m_elements[row * R + col];
    }
};

template<typename T>
using GridElements = ElementsContainer<T, 9, 9>;

using GridTiles = GridElements<std::shared_ptr<Tile>>;

#endif // __GRIDELEMENTS_H__