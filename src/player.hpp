#pragma once

#include "imports.hpp"
#include "gl.hpp"


namespace shrekrooms {


namespace controls {

    // player attributes    
    constexpr float mouseSensitivity    = 8.0f;
    constexpr float walkSpeed           = 3.0f;

    // keybinds
    constexpr int keyForward    = GLFW_KEY_W;
    constexpr int keyBackward   = GLFW_KEY_S;
    constexpr int keyLeft       = GLFW_KEY_A;
    constexpr int keyRight      = GLFW_KEY_D;

} // namespace controls


class Player {
public:
    Player(const gl::GLContext &glc, const glm::vec3 &pos, float cameraRot) : 
        m_glc(glc), m_uniman(glc.getUniformManager()), m_pos(pos), m_cameraRot(cameraRot) { }

    void update(float dt) {
        glm::vec2 mousepos = m_glc.getCursorPos();
        m_glc.setCursorPos({ 0.0f, 0.0f });

        m_cameraRot += controls::mouseSensitivity * mousepos.x * dt;

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
            m_pos += controls::walkSpeed * dPos * dt;
        }

        m_glc.enableShader();
        glm::mat4 view = glm::lookAt(m_pos, m_pos + forw, gl::globalUp);
        m_uniman.setViewMatrix(view);
    }

protected:
    const gl::GLContext &m_glc;
    const gl::UniformManager &m_uniman;
    glm::vec3 m_pos;
    float m_cameraRot;
};


} // namespace shrekrooms
