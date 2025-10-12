#include "shrek.hpp"

using namespace shrekrooms;


Shrek::Shrek(const gl::GLContext &glc, const TextureManager &texman, const maze::Maze &maze) : 
        m_glc(glc), m_uniman(glc.getUniformManager()), m_maze(maze) {
    m_tex = texman.getTexture(TextureManager::TextureID::Shrek);
    m_generateGeometry();
}

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
    m_uniman.useTexture(m_tex);
    glBindVertexArray(m_geometry.vao);
    glDrawArrays(GL_TRIANGLES, 0, defines::quadVertCount*1);
    m_uniman.setRotateMatrix(defines::mat4identity);
}

bool Shrek::isCollidingPlayer(const Player &player) const {
    return glm::distance(player.getPos(), m_pos) < (defines::shrek::radius + defines::player::radius);
}

void Shrek::m_generateGeometry() {
    GLuint vao, vbo;

    const size_t stride = 6;
    const size_t floatcount = stride*defines::quadVertCount;
    const float smax = 0.5f * defines::shrek::width;
    const float ymin = 0.5f * defines::world::chunkHeight;
    const float ymax = -ymin + defines::shrek::height;

    float verts[floatcount] = {
        0.0f, -ymin, -smax,    0.0f, 1.0f,     0.0f,
        0.0f, -ymin,  smax,    1.0f, 1.0f,     0.0f,
        0.0f,  ymax,  smax,    1.0f, 0.0f,     0.0f,
        0.0f, -ymin, -smax,    0.0f, 1.0f,     0.0f,
        0.0f,  ymax,  smax,    1.0f, 0.0f,     0.0f,
        0.0f,  ymax, -smax,    0.0f, 0.0f,     0.0f,
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, floatcount*sizeof(GLfloat), verts, GL_STATIC_DRAW);
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    // Texture id
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));

    m_geometry = gl::Geometry { vao, vbo };
}
