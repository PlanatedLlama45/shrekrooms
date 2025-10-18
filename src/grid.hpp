#pragma once

#include "imports.hpp"


namespace shrekrooms {


template <typename _Val>
class Grid {
public:
    struct Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_value = ptrdiff_t;
        using value_type = _Val;
        using pointer = _Val *;
        using reference = _Val &;

        Iterator(pointer ptr) : m_ptr(ptr) { }

        reference operator *() const { return *m_ptr; }
        pointer operator ->() { return m_ptr; }

        Iterator &operator ++() { m_ptr++; return *this; }
        Iterator operator ++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        Iterator &operator --() { m_ptr--; return *this; }
        Iterator operator --(int) { Iterator tmp = *this; --(*this); return tmp; }

        Iterator &operator +(difference_value offset) { m_ptr += offset; return *this; }
        Iterator &operator -(difference_value offset) { m_ptr -= offset; return *this; }

        difference_value operator -(const Iterator &it) { return m_ptr - it.m_ptr; }

        friend bool operator ==(const Iterator &it1, const Iterator &it2) { return (it1.m_ptr == it2.m_ptr); }
        friend bool operator !=(const Iterator &it1, const Iterator &it2) { return (it1.m_ptr != it2.m_ptr); }

    private:
        pointer m_ptr;
    };


    Grid(size_t width, size_t height, _Val val = _Val()) :
            m_width(width), m_height(height), m_size(width * height) {
        m_data = new _Val[m_size];
        for (size_t i = 0; i < m_size; i++)
            m_data[i] = val;
    }

    ~Grid() {
        delete[] m_data;
    }

    // Getters
    size_t width() const {
        return m_width;
    }

    size_t height() const {
        return m_width;
    }

    // Iterators
    Iterator begin() {
        return { m_data };
    }

    Iterator end() {
        return { m_data + m_size };
    }

    // Value indexing
    const _Val &at(size_t x, size_t y) const {
        if (x < 0 || x >= m_width)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'x' was out of range" };
        if (y < 0 || y >= m_height)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'y' was out of range" };
        return m_data[x + (y * m_height)];
    }

    const _Val &at(const glm::ivec2 &pos) const {
        return at(pos.x, pos.y);
    }

    _Val &operator [](const glm::ivec2 &pos) {
        if (pos.x < 0 || pos.x >= m_width)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'x' was out of range" };
        if (pos.y < 0 || pos.y >= m_height)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'y' was out of range" };
        return m_data[pos.x + (pos.y * m_height)];
    }

protected:
    _Val *m_data;
    size_t m_width, m_height, m_size;
    
};


} // namespace shrekrooms
