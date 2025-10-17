#pragma once

#include "defines.hpp"
#include "glc.hpp"
#include "managers.hpp"
#include "maze.hpp"


namespace shrekrooms {


glm::ivec2 worldToChunkCoords(const glm::vec2 &pos);
glm::ivec2 worldToChunkCoords(const glm::vec3 &pos);


struct Collision {
    bool isColliding;
    glm::vec2 cancelVector;

    Collision();
    Collision(bool isColliding, glm::vec2 cancelVector);

    void addOtherCollision(const Collision &coll);

};


struct Hitbox {
    glm::vec2 posMin, posMax;

    Hitbox();
    Hitbox(const glm::vec2 &posMin, const glm::vec2 &posMax);
    
    bool isInside(const glm::vec2 &pos) const;
    
    Collision getCircleIntersection(const glm::vec2 &pos, float radius) const;

};


class Chunk {
public:
    Chunk(const UniformManager &uniman, const MeshManager &meshman, const glm::vec2 &chunkPos, const maze::MazeNode &node);
    
    void draw() const;

    bool playerCanCollide(const glm::vec2 &pos) const;
    void addThisToCollision(Collision &coll, const glm::vec2 &pos, float radius) const;
    Collision getCollision(const glm::vec2 &pos, float radius) const;

protected:
    static constexpr size_t s_wallCount = 4;

    const UniformManager &m_uniman;
    const MeshManager &m_meshman;
    glm::vec2 m_chunkPos, m_chunkOffset;
    glm::ivec2 m_chunkPosI;
    glm::mat4 m_chunkTranslateMat;
    std::array<MeshManager::Mesh, (s_wallCount + 1)> m_meshes;
    /*
     * 0: x+
     * 1: x-
     * 2: z+
     * 3: z-
    */
    std::array<bool, s_wallCount> m_walls;
    std::array<Hitbox, s_wallCount> m_hitboxes;

    void m_setWalls(const maze::MazeNode &node);
    void m_addMeshes();
    void m_genHitboxes();

};


class World {
public:
    World(const gl::GLContext &glc, const MeshManager &meshman, const maze::Maze &maze);

    void draw() const;
    
    Collision getCollision(const glm::vec2 &pos, float radius) const;

protected:
    const gl::GLContext &m_glc;
    const UniformManager &m_uniman;
    const MeshManager &m_meshman;
    const maze::Maze &m_maze;
    std::vector<Chunk> m_chunks;

};


} // namespace shrekrooms
