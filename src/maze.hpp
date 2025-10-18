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

Direction operator |(Direction d1, Direction d2);
Direction operator &(Direction d1, Direction d2);
Direction operator ^(Direction d1, Direction d2);
Direction &operator |=(Direction &d1, Direction d2);
Direction &operator &=(Direction &d1, Direction d2);
Direction &operator ^=(Direction &d1, Direction d2);

glm::ivec2 getDirectionVector(Direction d);
Direction negateDirection(Direction d);

const std::array<const Direction, 4> allDirections {
    Direction::XPos,
    Direction::XNeg,
    Direction::ZPos,
    Direction::ZNeg
};

struct MazeNode {
    Direction walls;

    MazeNode();

    bool hasWall(Direction dir) const;

};


class Maze {
public:
    Maze(size_t size, float bridgePercent);

    const MazeNode &getNode(const glm::ivec2 &pos) const;

protected:
    Grid<MazeNode> m_nodes;

    Direction m_getRngDirection(const glm::ivec2 &pos, bool nextShouldBeEmpty);
    void m_generateMainPath();
    void m_addBridges(size_t size, float bridgePercent);

};


} // namespace shrekrooms::maze
