#pragma once

#include "imports.hpp"


namespace shrekrooms {


template <class T>
class Grid {
public:
    struct Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_value = ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

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


    Grid(size_t width, size_t height) :
            m_width(width), m_height(height), m_size(width * height) {
        m_data = new T[m_size];
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
    const T &at(size_t x, size_t y) const {
        if (x < 0 || x >= m_width)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'x' was out of range" };
        if (y < 0 || y >= m_height)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'y' was out of range" };
        return m_data[x + (y * m_height)];
    }

    const T &at(const glm::ivec2 &pos) const {
        return at(pos.x, pos.y);
    }

    T &operator [](const glm::ivec2 &pos) {
        if (pos.x < 0 || pos.x >= m_width)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'x' was out of range" };
        if (pos.y < 0 || pos.y >= m_height)
            throw error { "grid.hpp", "shrekrooms::Grid::at", "'y' was out of range" };
        return m_data[pos.x + (pos.y * m_height)];
    }

protected:
    T *m_data;
    size_t m_width, m_height, m_size;
    
};


} // namespace shrekrooms
