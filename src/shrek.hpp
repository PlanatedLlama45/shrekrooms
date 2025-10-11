#pragma once

#include "imports.hpp"
#include "player.hpp"


namespace shrekrooms {


namespace shrek_data {

    constexpr float width       = 3.0f;
    constexpr float height      = 4.0f;
    constexpr float radius      = 1.5f;

    constexpr float walkSpeed   = 7.0f;

    constexpr float baseMoveTime = 6.0f;
    constexpr float moveTimeAmp  = 1.5f;
    constexpr float baseWaitTime = 1.0f;
    constexpr float waitTimeAmp  = 0.5f;

} // namespace shrek_data


class Shrek {
public:
    Shrek(const gl::GLContext &glc, const glm::vec3 &pos, const gl::Texture &tex) : 
            m_glc(glc), m_uniman(glc.getUniformManager()), m_pos(pos), m_tex(tex) {
        m_generateGeometry();
        m_cooldownProgress = 0;
        m_cooldownTime = shrek_data::baseWaitTime;
        m_isWaiting = true;
    }

    ~Shrek() {
        glDeleteVertexArrays(1, &m_geometry.vao);
        glDeleteBuffers(1, &m_geometry.vbo);
    }

    void update(const World &world, const Player &player, float dt) {
        m_cooldownProgress += dt;
        if (m_cooldownProgress >= m_cooldownTime) {
            m_cooldownProgress = 0;
            m_isWaiting = !m_isWaiting;
            if (m_isWaiting)
                m_cooldownTime = shrek_data::baseWaitTime + getRandomFloat(-shrek_data::moveTimeAmp, shrek_data::moveTimeAmp);
            else
                m_cooldownTime = shrek_data::baseMoveTime + getRandomFloat(-shrek_data::waitTimeAmp, shrek_data::waitTimeAmp);
        }

        if (m_isWaiting)
            return;

        glm::vec3 dPos = glm::normalize(player.getPos() - m_pos);
        m_pos += dPos * shrek_data::walkSpeed * dt;

        auto inter = world.getCircleIntersection({ m_pos.x, m_pos.z }, shrek_data::radius);
        if (inter.first)
            m_pos += glm::vec3 { inter.second.x, 0.0f, inter.second.y };
    }

    void draw(const Player &player) const {
        glm::vec3 playerPos = player.getPos();

        // Position based
        float angle = glm::acos(glm::dot(glm::normalize(m_pos - playerPos), { 1.0f, 0.0f, 0.0f }));
        if (m_pos.z - playerPos.z < 0) angle = -angle;
        glm::mat4 rotMat = glm::rotate(gl::mat4identity, -angle, gl::globalUp);

        // View direction based
        // glm::mat4 rotMat = glm::rotate(gl::mat4identity, -player.getCameraRot(), gl::globalUp);

        m_uniman.setRotateMatrix(rotMat);

        glm::mat4 transMat = glm::translate(gl::mat4identity, m_pos);

        m_uniman.setTranslateMatrix(transMat);
        m_uniman.setTexture(0, m_tex);
        glBindVertexArray(m_geometry.vao);
        glDrawArrays(GL_TRIANGLES, 0, gl::quadVertCount*1);
        m_uniman.setRotateMatrix(gl::mat4identity);
    }

    bool isCollidingPlayer(const Player &player) const {
        return glm::distance(player.getPos(), m_pos) < (shrek_data::radius + player_data::radius);
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    const gl::Texture &m_tex;
    glm::vec3 m_pos;
    gl::Geometry m_geometry;
    float m_cooldownProgress;
    float m_cooldownTime;
    bool m_isWaiting;

    void m_generateGeometry() {
        GLuint vao, vbo;

        constexpr size_t stride = 6;
        constexpr size_t floatcount = stride*gl::quadVertCount;
        constexpr float smax = 0.5f * shrek_data::width;
        constexpr float ymin = 0.5f * world_data::chunkHeight;
        constexpr float ymax = -ymin + shrek_data::height;

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
};


} // namespace shrekrooms
