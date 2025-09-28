#pragma once

#include "imports.hpp"
#include "grid.hpp"

#define _MAZE_DESMOS_OUTPUT


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
    case Direction::XPos: return Direction::XNeg;
    case Direction::XNeg: return Direction::XPos;
    case Direction::ZPos: return Direction::ZNeg;
    case Direction::ZNeg: return Direction::ZPos;
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
    Maze(size_t size, float bridgePercent) :
            m_nodes(size, size) {
        m_generateMainPath();
        m_addBridges(size, bridgePercent);
#ifdef _MAZE_DESMOS_OUTPUT
        const std::array<const Direction, 4> allDirs {
            Direction::XPos,
            Direction::XNeg,
            Direction::ZPos,
            Direction::ZNeg
        };
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                glm::ivec2 pos { x, y };
                MazeNode node = m_nodes[pos];
                std::cout << '(' << pos.x << ',' << pos.y << ')';
                for (Direction d : allDirs) {
                    if ((node.walls & d) == Direction::Null) {
                        auto nxPos = pos + getDirectionVector(d);
                        glm::vec2 hpos = (static_cast<glm::vec2>(nxPos) + static_cast<glm::vec2>(pos)) * 0.5f;
                        std::cout << ",(" << hpos.x << ',' << hpos.y << "),(" << nxPos.x << ',' << nxPos.y << "),(" << pos.x << ',' << pos.y << "),(" << hpos.x << ',' << hpos.y << ')';
                    }
                }
                std::cout << '\n';
            }
        }
#endif
    };

protected:
    Grid<MazeNode> m_nodes;

    Direction m_getRngDirection(const glm::ivec2 &pos, bool nextShouldBeEmpty) {
        static const std::array<const Direction, 4> allDirs {
            Direction::XPos,
            Direction::XNeg,
            Direction::ZPos,
            Direction::ZNeg
        };
        
        std::vector<Direction> res;
        res.reserve(4);

        if (nextShouldBeEmpty) {
            for (Direction d : allDirs) {
                try {
                    MazeNode &node = m_nodes[pos + getDirectionVector(d)];
                    if (node.walls == Direction::All)
                        res.push_back(d);
                } catch (error &err) {
                    continue;
                }
            }
        } else {
            Direction thisWalls = m_nodes[pos].walls;
            for (Direction d : allDirs) {
                try {
                    m_nodes[pos + getDirectionVector(d)];
                } catch (error &err) {
                    continue;
                }
                if ((thisWalls & d) != Direction::Null)
                    res.push_back(d);
            }
        }
        if (res.empty())
            return Direction::Null;
        return res[rand() % res.size()];
    }

    void m_generateMainPath() {
        std::stack<glm::ivec2> pos;
        pos.push({ 0, 0 });
        while (true) {
            Direction dir = m_getRngDirection(pos.top(), true);
            if (dir == Direction::Null) {
                pos.pop();
                if (pos.empty())
                    break;
                continue;
            }
            m_nodes[pos.top()].walls ^= dir;
            pos.push(pos.top() + getDirectionVector(dir));
            m_nodes[pos.top()].walls ^= negateDirection(dir);
        }
    }

    void m_addBridges(size_t size, float bridgePercent) {
        if (bridgePercent < 0.0f || bridgePercent > 1.0f)
            throw error { "maze.hpp", "shrekrooms::maze::Maze::m_addBridges", "'bridgePercent' has to be in [0;1]" };

        const size_t bridgeCount = static_cast<size_t>(size*size * bridgePercent);
        for (size_t i = 0; i < bridgeCount; i++) {
            glm::ivec2 pos { rand()%size, rand()%size };
            Direction dir = m_getRngDirection(pos, false);
            if (dir == Direction::Null) {
                i--;
                continue;
            }

            m_nodes[pos].walls ^= dir;
            m_nodes[pos + getDirectionVector(dir)].walls ^= negateDirection(dir);
        }
    }

};


} // namespace shrekrooms::maze
