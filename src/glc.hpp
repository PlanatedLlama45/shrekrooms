#pragma once

#include "defines.hpp"
#include "shaders.hpp"
#include "managers.hpp"
#include "gl_util.hpp"


namespace shrekrooms::gl {


void _m_glc_errorCallback(int error_code, const char *description) {
    std::cerr << "GLFW ran into an error\nError code: " << error_code << ":\n" << description << std::endl;
}


class GLContext {
public:
    struct Window {
        GLFWwindow *ptr;
        int width;
        int height;

        glm::ivec2 getSizeVector() const {
            return { width, height };
        }

        float getAspectRatio() const {
            return static_cast<float>(width) / static_cast<float>(height);
        }
    };


    GLContext(int width, int height, const char *title, bool windowResizeable = false, int exitKey = GLFW_KEY_UNKNOWN) :
            m_exitKey(exitKey) {
        if (!glfwInit())
            throw error { "gl.hpp", "shrekrooms::gl::GLContext::GLContext", "Failed to initialize GLFW" };

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

        m_window.ptr = glfwCreateWindow(width, height, title, NULL, NULL);

        if (!m_window.ptr) {
            glfwTerminate();
            throw error { "gl.hpp", "shrekrooms::gl::GLContext::GLContext", "Failed to create window" };
        }

        glfwSetWindowAttrib(m_window.ptr, GLFW_RESIZABLE, GL_FALSE);
        glfwMakeContextCurrent(m_window.ptr);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();
            glfwDestroyWindow(m_window.ptr);
            throw error { "gl.hpp", "shrekrooms::gl::GLContext::GLContext", "Failed to initialize GLAD" };
        }

        glfwGetFramebufferSize(m_window.ptr, &m_window.width, &m_window.height);
        glViewport(0, 0, m_window.width, m_window.height);

        glfwSetErrorCallback(_m_glc_errorCallback);

        m_shader = shaders::makeShaderProgram();
        m_uniman = std::make_unique<UniformManager>(m_shader);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        enableShader();
        glm::mat4 projMat = glm::perspective(defines::deg2rad*90.0f, m_window.getAspectRatio(), 0.1f, 15.0f);
        m_uniman->setProjectionMatrix(projMat);
        m_uniman->setTranslateMatrix(defines::mat4identity);
    }

    ~GLContext() {
        glDeleteProgram(m_shader);
        glfwDestroyWindow(m_window.ptr);
        glfwTerminate();
    }

    // Getters/setters
    const Window &getWindow() const {
        return m_window;
    }

    const UniformManager &getUniformManager() const {
        return *m_uniman;
    }

    GLuint getShader() const {
        return m_shader;
    }

    // General
    bool isRunning() const {
        if (isKeyPressed(m_exitKey))
            return false;
        return !glfwWindowShouldClose(m_window.ptr);
    }

    void pollEvents() const {
        glfwPollEvents();
    }

    bool isWindowFocused() const {
        return glfwGetWindowAttrib(m_window.ptr, GLFW_FOCUSED);
    }

    // Drawing
    void setBackgroundColor(const Color &col) const {
        glClearColor(col.r, col.g, col.b, col.a);
    }

    void clearBackground() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void enableShader() const {
        glUseProgram(m_shader);
    }

    void drawBuffer() const {
        glfwSwapBuffers(m_window.ptr);
    }

    // Keyboard
    bool isKeyPressed(int key) const {
        return glfwGetKey(m_window.ptr, key) == GLFW_PRESS;
    }

    bool isKeyPressedRepeat(int key) const {
        return glfwGetKey(m_window.ptr, key) == GLFW_REPEAT;
    }

    bool isKeyReleased(int key) const {
        return glfwGetKey(m_window.ptr, key) == GLFW_RELEASE;
    }
    
    // Mouse
    bool isMouseButtonPressed(int btn) const {
        return glfwGetMouseButton(m_window.ptr, btn) == GLFW_PRESS;
    }

    bool isMouseButtonClicked(int btn) const {
        static std::array<bool, 8> m_mouseButtonPressedPrev { false };

        bool pressed = isMouseButtonPressed(btn);
        bool res = pressed && !m_mouseButtonPressedPrev[btn];
        m_mouseButtonPressedPrev[btn] = pressed;
        return res;
    }
    
    bool isMouseButtonReleased(int btn) const {
        return glfwGetMouseButton(m_window.ptr, btn) == GLFW_RELEASE;
    }

    glm::vec2 getCursorPos() const {
        double x, y;
        glfwGetCursorPos(m_window.ptr, &x, &y);
        return { x - 0.5f*m_window.width, -y + 0.5f*m_window.height };
    }

    void setCursorPos(glm::vec2 pos) const {
        glfwSetCursorPos(m_window.ptr, pos.x + 0.5f*m_window.width, -pos.y + 0.5f*m_window.height);
    }

    void hideCursor() const {
        glfwSetInputMode(m_window.ptr, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void showCursor() const {
        glfwSetInputMode(m_window.ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

protected:
    GLuint m_shader;
    Window m_window;
    std::unique_ptr<UniformManager> m_uniman;
    int m_exitKey;

};


} // namespace shrekrooms::gl
