#pragma once

#include "defines.hpp"
#include "player.hpp"
#include "maze.hpp"


namespace shrekrooms {


class Shrek {
public:
    Shrek(const gl::GLContext &glc, const maze::Maze &maze, const gl::Texture &tex) : 
            m_glc(glc), m_uniman(glc.getUniformManager()), m_maze(maze), m_tex(tex) {
        m_generateGeometry();
        m_cooldownProgress = 0;
        m_cooldownTime = defines::shrek::baseWaitTime;
        m_isWaiting = true;
    }

    void update(const World &world, const Player &player, float dt) {
        m_cooldownProgress += dt;
        if (m_cooldownProgress >= m_cooldownTime) {
            m_cooldownProgress = 0;
            m_isWaiting = !m_isWaiting;
            if (m_isWaiting)
                m_cooldownTime = defines::shrek::baseWaitTime + getRandomFloat(-defines::shrek::moveTimeAmp, defines::shrek::moveTimeAmp);
            else
                m_cooldownTime = defines::shrek::baseMoveTime + getRandomFloat(-defines::shrek::waitTimeAmp, defines::shrek::waitTimeAmp);
        }

        if (m_isWaiting)
            return;

        glm::vec3 dPos = glm::normalize(player.getPos() - m_pos);
        m_pos += dPos * defines::shrek::walkSpeed * dt;

        // auto inter = world.getCircleIntersection({ m_pos.x, m_pos.z }, defines::shrek::radius);
        // if (inter.first)
        //     m_pos += glm::vec3 { inter.second.x, 0.0f, inter.second.y };
    }

    void draw(const Player &player) const {
        glm::vec3 playerPos = player.getPos();

        // Position based
        float angle = glm::acos(glm::dot(glm::normalize(m_pos - playerPos), { 1.0f, 0.0f, 0.0f }));
        if (m_pos.z - playerPos.z < 0) angle = -angle;
        glm::mat4 rotMat = glm::rotate(defines::mat4identity, -angle, defines::globalUp);

        // View direction based
        // glm::mat4 rotMat = glm::rotate(defines::mat4identity, -player.getCameraRot(), defines::globalUp);

        // m_uniman.setRotateMatrix(rotMat);

        glm::mat4 transMat = glm::translate(defines::mat4identity, m_pos);

        m_uniman.setTranslateMatrix(transMat);
        // m_uniman.useTexture(0, m_tex);
        glBindVertexArray(m_geometry.vao);
        glDrawArrays(GL_TRIANGLES, 0, defines::quadVertCount*1);
        // m_uniman.setRotateMatrix(defines::mat4identity);
    }

    bool isCollidingPlayer(const Player &player) const {
        return glm::distance(player.getPos(), m_pos) < (defines::shrek::radius + defines::player::radius);
    }

protected:
    const gl::GLContext &m_glc;
    const UniformManager &m_uniman;
    const maze::Maze &m_maze;
    const gl::Texture &m_tex;
    glm::vec3 m_pos;
    gl::Geometry m_geometry;
    float m_cooldownProgress;
    float m_cooldownTime;
    bool m_isWaiting;

    void m_generateGeometry() {
        GLuint vao, vbo;

        constexpr size_t stride = 6;
        constexpr size_t floatcount = stride*defines::quadVertCount;
        constexpr float smax = 0.5f * defines::shrek::width;
        constexpr float ymin = 0.5f * defines::world::chunkHeight;
        constexpr float ymax = -ymin + defines::shrek::height;

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
