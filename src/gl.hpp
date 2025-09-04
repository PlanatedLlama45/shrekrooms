#pragma once

#include "imports.hpp"
#include "shaders.hpp"

namespace shrekrooms::gl {


const glm::vec3 globalUp { 0.0f, 1.0f, 0.0f };
constexpr float deg2rad = M_PI / 180.0f;
const glm::mat4 mat4identity { 1.0f };
constexpr int quadVertCount = 6;


struct Color : public glm::vec4 {
    Color(float r, float g, float b, float a = 1.0f) :
        glm::vec4(r, g, b, a) {}

    Color(const glm::vec4 &v) :
        glm::vec4(v) {}
};


struct Geometry {
    Geometry() :
        vao(0), vbo(0) { }

    Geometry(GLuint vao, GLuint vbo) :
        vao(vao), vbo(vbo) { }

    GLuint vao, vbo;
};


struct Texture {
public:
    // Defined in gl.cpp
    Texture(const std::string &filename, bool interpolation = true);

    ~Texture() {
        if (m_tex.use_count() == 1)
            glDeleteTextures(1, m_tex.get());
    }    

    int getWidth() const {
        return m_width;
    }

    int getHeight() const {
        return m_height;
    }

    glm::ivec2 getSize() const {
        return { m_width, m_height };
    }

    GLuint getId() const {
        return *m_tex;
    }

protected:
    std::shared_ptr<GLuint> m_tex;
    int m_width, m_height;

};


class UniformManager {
public:
    enum Uniform {
        UNIMAN_Translate = 0,
        UNIMAN_View,
        UNIMAN_Projection,
        UNIMAN_Color,
        UNIMAN_ViewPos,
        UNIMAN_FogColor
    };

    UniformManager(GLuint shader) :
            m_shader(shader) {
        m_uniforms[UNIMAN_Translate]  = m_getUniformLocation("translate");
        m_uniforms[UNIMAN_View]       = m_getUniformLocation("view");
        m_uniforms[UNIMAN_Projection] = m_getUniformLocation("projection");
        m_uniforms[UNIMAN_Color]      = m_getUniformLocation("color");
        m_uniforms[UNIMAN_ViewPos]    = m_getUniformLocation("viewPos");
        m_uniforms[UNIMAN_FogColor]    = m_getUniformLocation("fogColor");
    }

    // Uniforms
    void setTexture(GLuint id, const Texture &tex) const {
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, tex.getId());
    }

    void setTranslateMatrix(const glm::mat4 &translateMat) const {
        glUniformMatrix4fv(m_uniforms[UNIMAN_Translate], 1, GL_FALSE, glm::value_ptr(translateMat));
    }

    void setViewMatrix(const glm::mat4 &viewMat) const {
        glUniformMatrix4fv(m_uniforms[UNIMAN_View], 1, GL_FALSE, glm::value_ptr(viewMat));
    }

    void setProjectionMatrix(const glm::mat4 &projectionMat) const {
        glUniformMatrix4fv(m_uniforms[UNIMAN_Projection], 1, GL_FALSE, glm::value_ptr(projectionMat));
    }

    void setColor(const Color &color) const {
        glUniform4fv(m_uniforms[UNIMAN_Color], 1, glm::value_ptr(static_cast<glm::vec4>(color)));
    }

    void setFogColor(const Color &color) const {
        glUniform4fv(m_uniforms[UNIMAN_FogColor], 1, glm::value_ptr(static_cast<glm::vec4>(color)));
    }

    void setViewPos(const glm::vec3 &pos) const {
        glUniform3fv(m_uniforms[UNIMAN_ViewPos], 1, glm::value_ptr(pos));
    }

protected:
    GLuint m_shader;
    std::array<GLuint, 6> m_uniforms;

    GLuint m_getUniformLocation(const std::string &name) const {
        return glGetUniformLocation(m_shader, name.c_str());
    }
};


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

        m_makeShaderProgram();
        glEnable(GL_DEPTH_TEST);
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
    void clearBackground(const Color &col) const {
        glClearColor(col.r, col.g, col.b, col.a);
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

    GLuint m_makeShaderModule(const char *shaderSrc, GLuint moduleType) {
        GLuint shaderModule = glCreateShader(moduleType);
        glShaderSource(shaderModule, 1, &shaderSrc, NULL);
        glCompileShader(shaderModule);

        int success;
        glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);

        if (!success) {
            char errorLog[1024];
            glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
            throw error { "gl.hpp", "shrekrooms::gl::GLContext::m_makeShaderModule", "Shader Module compilation error:\n" } << errorLog;
        }

        return shaderModule;
    }

    void m_makeShaderProgram() {
        std::string vertex_s;
        std::string fragment_s;
        
        std::ifstream vert("../src/shaders/vertex.glsl");
        std::string line;
        while (std::getline(vert, line))
            vertex_s += line + '\n';
        vert.close();

        std::ifstream frag("../src/shaders/fragment.glsl");
        while (std::getline(frag, line))
            fragment_s += line + '\n';
        frag.close();

        GLuint vertexModule = m_makeShaderModule(vertex_s.c_str(), GL_VERTEX_SHADER);
        GLuint fragmentModule = m_makeShaderModule(fragment_s.c_str(), GL_FRAGMENT_SHADER);

        m_shader = glCreateProgram();

        glAttachShader(m_shader, vertexModule);
        glAttachShader(m_shader, fragmentModule);

        glLinkProgram(m_shader);

        int success;
        glGetProgramiv(m_shader, GL_LINK_STATUS, &success);

        if (!success) {
            char errorLog[1024];
            glGetProgramInfoLog(m_shader, 1024, NULL, errorLog);
            throw error { "gl.hpp", "shrekrooms::gl::GLContext::m_makeShaderProgram", "Shader linking error:\n" } << errorLog;
        }

        glDeleteShader(vertexModule);
        glDeleteShader(fragmentModule);

        // Uniforms
        m_uniman = std::make_unique<UniformManager>(m_shader);
    }

};


} // namespace shrekrooms::gl
