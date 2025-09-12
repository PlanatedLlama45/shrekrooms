#pragma once

#include "imports.hpp"
#include "grid.hpp"


namespace shrekrooms::maze {


enum class Direction : int {
    Null = 0b0000,

    XPos = 0b1000,
    XNeg = 0b0100,
    ZPos = 0b0010,
    ZNeg = 0b0001,

    All = 0b1111
};

Direction operator |(Direction d1, Direction d2) {
    return static_cast<Direction>(static_cast<int>(d1) | static_cast<int>(d2));
}

Direction operator &(Direction d1, Direction d2) {
    return static_cast<Direction>(static_cast<int>(d1) & static_cast<int>(d2));
}

Direction operator ^(Direction d1, Direction d2) {
    return static_cast<Direction>(static_cast<int>(d1) ^ static_cast<int>(d2));
}

Direction &operator |=(Direction &d1, Direction d2) {
    d1 = d1 | d2;
    return d1;
}

Direction &operator &=(Direction &d1, Direction d2) {
    d1 = d1 & d2;
    return d1;
}

Direction &operator ^=(Direction &d1, Direction d2) {
    d1 = d1 ^ d2;
    return d1;
}


glm::ivec2 getDirectionVector(Direction d) {
    switch (d) {
    case Direction::XPos: return {  1,  0 };
    case Direction::XNeg: return { -1,  0 };
    case Direction::ZPos: return {  0,  1 };
    case Direction::ZNeg: return {  0, -1 };
    }
    return { 0, 0 };
}

Direction negateDirection(Direction d) {
    switch (d) {
    case Direction::XPos: Direction::XNeg;
    case Direction::XNeg: Direction::XPos;
    case Direction::ZPos: Direction::ZNeg;
    case Direction::ZNeg: Direction::ZPos;
    }
    return Direction::Null;
}


struct MazeNode {
    MazeNode() :
        walls(Direction::All) { }

    Direction walls;
};


class Maze {
public:
    Maze(size_t size) :
            m_nodes(size, size) {
        m_generateMaze();
    };

protected:
    Grid<MazeNode> m_nodes;

    Direction m_getRngDirection(const glm::ivec2 &pos) {
        static const std::array<const Direction, 4> allDirs {
            Direction::XPos,
            Direction::XNeg,
            Direction::ZPos,
            Direction::ZNeg
        };
        
        std::vector<Direction> res;
        res.reserve(4);
        
        for (Direction d : allDirs) {
            try {
                MazeNode &node = m_nodes[pos + getDirectionVector(d)];
                if (node.walls == Direction::All)
                    res.push_back(d);
            } catch (error &err) {
                continue;
            }
        }
        if (res.empty())
            return Direction::Null;
        return res[rand() % res.size()];
    }

    void m_generateMaze() {
        glm::ivec2 pos { 0, 0 };
        while (true) {
            std::cout << '(' << pos.x << ',' << pos.y << ')';

            Direction dir = m_getRngDirection(pos);
            if (dir == Direction::Null)
                break;
            std::cout << ',';
            m_nodes[pos].walls ^= dir;
            pos += getDirectionVector(dir);
            m_nodes[pos].walls ^= negateDirection(dir);
        }
    }
};


} // namespace shrekrooms::maze
