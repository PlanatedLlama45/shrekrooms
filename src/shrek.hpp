#pragma once

#include "defines.hpp"
#include "player.hpp"
#include "maze.hpp"


namespace shrekrooms {


class Shrek {
public:
    Shrek(const gl::GLContext &glc, const TextureManager &texman, const maze::Maze &maze);

    void update(const World &world, const Player &player, float dt);
    void draw(const Player &player) const;
    bool isCollidingPlayer(const Player &player) const;

protected:
    const gl::GLContext &m_glc;
    const UniformManager &m_uniman;
    const maze::Maze &m_maze;
    gl::Texture m_tex;
    glm::vec3 m_pos;
    gl::Geometry m_geometry; // TODO: Move to MeshManager
    float m_cooldownProgress;
    float m_cooldownTime;
    bool m_isWaiting;

    void m_generateGeometry();

};


} // namespace shrekrooms
