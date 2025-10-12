#pragma once

#include "defines.hpp"
#include "shaders.hpp"
#include "managers.hpp"
#include "gl_util.hpp"


namespace shrekrooms::gl {


void _m_errorCallback(int errorCode, const char *description);


class GLContext {
public:
    struct Window {
        GLFWwindow *ptr;
        int width;
        int height;

        glm::ivec2 getSizeVector() const;
        float getAspectRatio() const;

    };


    GLContext(int width, int height, const char *title, bool windowResizeable = false, int exitKey = GLFW_KEY_UNKNOWN);

    ~GLContext();

    // Getters/setters
    const Window &getWindow() const;
    const UniformManager &getUniformManager() const;
    GLuint getShader() const;

    // General
    bool isRunning() const;
    void pollEvents() const;
    bool isWindowFocused() const;

    // Drawing
    void setBackgroundColor(const Color &col) const;
    void clearBackground() const;
    void enableShader() const;
    void drawBuffer() const;

    // Keyboard
    bool isKeyPressed(int key) const;
    bool isKeyPressedRepeat(int key) const;
    bool isKeyReleased(int key) const;
    
    // Mouse
    bool isMouseButtonPressed(int btn) const;
    bool isMouseButtonClicked(int btn) const;
    bool isMouseButtonReleased(int btn) const;
    glm::vec2 getCursorPos() const;
    void setCursorPos(glm::vec2 pos) const;
    void hideCursor() const;
    void showCursor() const;

protected:
    GLuint m_shader;
    Window m_window;
    std::unique_ptr<UniformManager> m_uniman;
    int m_exitKey;

};


} // namespace shrekrooms::gl
