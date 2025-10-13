#pragma once

#include "defines.hpp"
#include "player.hpp"
#include "maze.hpp"


namespace shrekrooms {


class Shrek {
public:
    Shrek(const gl::GLContext &glc, const MeshManager &meshman, const maze::Maze &maze, const glm::vec3 &pos);

    void update(const World &world, const Player &player, float dt);
    void draw(const Player &player) const;
    bool isCollidingPlayer(const Player &player) const;

protected:
    static constexpr MeshManager::Mesh s_mesh = MeshManager::Mesh::Shrek;
    const gl::GLContext &m_glc;
    const MeshManager &m_meshman;
    const UniformManager &m_uniman;
    const maze::Maze &m_maze;
    gl::Texture m_tex;
    glm::vec3 m_pos;
    float m_cooldownProgress;
    float m_cooldownTime;
    bool m_isWaiting;

};


} // namespace shrekrooms
