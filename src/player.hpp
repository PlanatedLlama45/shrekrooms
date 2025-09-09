#pragma once

#include "imports.hpp"
#include "gl.hpp"
#include "world.hpp"


namespace shrekrooms {


namespace player_data {

    constexpr float mouseSensitivity    = 10.0f;
    constexpr float walkSpeed           = 8.0f;
    constexpr float radius              = 1.0f;

} // namespace player_data

namespace controls {

    constexpr int keyForward    = GLFW_KEY_W;
    constexpr int keyBackward   = GLFW_KEY_S;
    constexpr int keyLeft       = GLFW_KEY_A;
    constexpr int keyRight      = GLFW_KEY_D;

} // namespace controls


class Player {
public:
    Player(const gl::GLContext &glc, const glm::vec3 &pos, float cameraRot) : 
        m_glc(glc), m_uniman(glc.getUniformManager()), m_pos(pos), m_cameraRot(cameraRot) { }

    const glm::vec3 &getPos() const {
        return m_pos;
    }

    float getCameraRot() const {
        return m_cameraRot;
    }

    void update(const World &world, float dt) {
        if (!m_glc.isWindowFocused())
            return;

        glm::vec2 mousepos = m_glc.getCursorPos();
        m_glc.setCursorPos({ 0.0f, 0.0f });

        m_cameraRot += player_data::mouseSensitivity * mousepos.x * dt;

        glm::vec3 forw = {
            glm::cos(m_cameraRot),
            0.0f,
            glm::sin(m_cameraRot)
        };
        glm::vec3 right = glm::normalize(glm::cross(forw, gl::globalUp));

        glm::vec3 dPos = { 0.0f, 0.0f, 0.0f };
        if (m_glc.isKeyPressed(controls::keyForward))
            dPos += forw;
        if (m_glc.isKeyPressed(controls::keyBackward))
            dPos -= forw;
        if (m_glc.isKeyPressed(controls::keyLeft))
            dPos -= right;
        if (m_glc.isKeyPressed(controls::keyRight))
            dPos += right;

        if (glm::length(dPos) > 0.1f) {
            dPos = glm::normalize(dPos);
            m_pos += player_data::walkSpeed * dPos * dt;
        }

        auto inter = world.getCircleIntersection({ m_pos.x, m_pos.z }, player_data::radius);
        if (inter.first)
            m_pos += glm::vec3 { inter.second.x, 0.0f, inter.second.y };

        m_glc.enableShader();
        glm::mat4 view = glm::lookAt(m_pos, m_pos + forw, gl::globalUp);
        m_uniman.setViewMatrix(view);

        m_uniman.setViewPos(m_pos);
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    glm::vec3 m_pos;
    float m_cameraRot;
};


} // namespace shrekrooms
