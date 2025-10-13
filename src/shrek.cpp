#include "shrek.hpp"

using namespace shrekrooms;


Shrek::Shrek(const gl::GLContext &glc, const MeshManager &meshman, const maze::Maze &maze, const glm::vec3 &pos) : 
        m_glc(glc), m_meshman(meshman), m_uniman(glc.getUniformManager()), m_maze(maze), m_pos(pos) { }

void Shrek::update(const World &world, const Player &player, float dt) {
    glm::vec3 dPos = glm::normalize(player.getPos() - m_pos);
    m_pos += dPos * defines::shrek::walkSpeed * dt;

    auto inter = world.getCollision({ m_pos.x, m_pos.z }, defines::shrek::radius);
    if (inter.isColliding)
        m_pos += glm::vec3 { inter.cancelVector.x, 0.0f, inter.cancelVector.y };
}

void Shrek::draw(const Player &player) const {
    glm::vec3 playerPos = player.getPos();

    // Position based
    float angle = glm::acos(glm::dot(glm::normalize(m_pos - playerPos), { 1.0f, 0.0f, 0.0f }));
    if (m_pos.z - playerPos.z < 0) angle = -angle;
    glm::mat4 rotMat = glm::rotate(defines::mat4identity, -angle, defines::globalUp);
    m_uniman.setRotateMatrix(rotMat);

    // View direction based
    // glm::mat4 rotMat = glm::rotate(defines::mat4identity, -player.getCameraRot(), defines::globalUp);

    // m_uniman.setRotateMatrix(rotMat);

    glm::mat4 transMat = glm::translate(defines::mat4identity, m_pos);

    m_uniman.setTranslateMatrix(transMat);
    m_meshman.renderMesh(s_mesh);
    m_uniman.setRotateMatrix(defines::mat4identity);
}

bool Shrek::isCollidingPlayer(const Player &player) const {
    return glm::distance(player.getPos(), m_pos) < (defines::shrek::radius + defines::player::radius);
}
