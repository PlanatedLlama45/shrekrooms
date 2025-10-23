#pragma once

#include "defines.hpp"
#include "player.hpp"
#include "maze.hpp"


namespace shrekrooms {


class Shrek {
public:
    Shrek(const gl::GLContext &glc, const maze::Maze &maze, const Player &player, const glm::ivec2 &mazePos);
    ~Shrek();

    void update(const World &world, float dt);
    void draw() const;
    bool isCollidingPlayer() const;

protected:
    static constexpr MeshManager::Mesh s_mesh = MeshManager::Mesh::Shrek;
    const gl::GLContext &m_glc;
    const MeshManager &m_meshman;
    const UniformManager &m_uniman;
    const maze::Maze &m_maze;
    const Player &m_player;
    gl::Texture m_tex;
    glm::vec3 m_pos;
    glm::ivec2 m_mazePos;
    std::deque<glm::ivec2> m_targetPath;

    std::thread m_pathFindThread;
    mutable std::condition_variable m_condVar;
    mutable std::mutex m_mutex;
    bool m_pathExhausted = false;

    void m_pathFind();
    std::deque<glm::ivec2> m_getShortestPath(const glm::ivec2 &start, const glm::ivec2 &end) const;

};


} // namespace shrekrooms
