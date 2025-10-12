#include "glc.hpp"

using namespace shrekrooms::gl;


void shrekrooms::gl::_m_errorCallback(int errorCode, const char *description) {
    std::cerr << "GLFW ran into an error\nError code: " << errorCode << ":\n" << description << std::endl;
}


/*
 * class shrekrooms::gl::GLContext
*/

GLContext::GLContext(int width, int height, const char *title, bool windowResizeable, int exitKey) :
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

    glfwSetErrorCallback(_m_errorCallback);

    m_shader = shaders::makeShaderProgram();
    m_uniman = std::make_unique<UniformManager>(m_shader);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    enableShader();
    glm::mat4 projMat = glm::perspective(defines::deg2rad*90.0f, m_window.getAspectRatio(), 0.1f, 15.0f);
    m_uniman->setProjectionMatrix(projMat);
    m_uniman->setTranslateMatrix(defines::mat4identity);
    m_uniman->setRotateMatrix(defines::mat4identity);
}

GLContext::~GLContext() {
    glDeleteProgram(m_shader);
    glfwDestroyWindow(m_window.ptr);
    glfwTerminate();
}

const GLContext::Window &GLContext::getWindow() const {
    return m_window;
}

const shrekrooms::UniformManager &GLContext::getUniformManager() const {
    return *m_uniman;
}

GLuint GLContext::getShader() const {
    return m_shader;
}

// General
bool GLContext::isRunning() const {
    if (isKeyPressed(m_exitKey))
        return false;
    return !glfwWindowShouldClose(m_window.ptr);
}

void GLContext::pollEvents() const {
    glfwPollEvents();
}

bool GLContext::isWindowFocused() const {
    return glfwGetWindowAttrib(m_window.ptr, GLFW_FOCUSED);
}

// Drawing
void GLContext::setBackgroundColor(const Color &col) const {
    glClearColor(col.r, col.g, col.b, col.a);
}

void GLContext::clearBackground() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLContext::enableShader() const {
    glUseProgram(m_shader);
}

void GLContext::drawBuffer() const {
    glfwSwapBuffers(m_window.ptr);
}

// Keyboard
bool GLContext::isKeyPressed(int key) const {
    return glfwGetKey(m_window.ptr, key) == GLFW_PRESS;
}

bool GLContext::isKeyPressedRepeat(int key) const {
    return glfwGetKey(m_window.ptr, key) == GLFW_REPEAT;
}

bool GLContext::isKeyReleased(int key) const {
    return glfwGetKey(m_window.ptr, key) == GLFW_RELEASE;
}

// Mouse
bool GLContext::isMouseButtonPressed(int btn) const {
    return glfwGetMouseButton(m_window.ptr, btn) == GLFW_PRESS;
}

bool GLContext::isMouseButtonClicked(int btn) const {
    static std::array<bool, 8> m_mouseButtonPressedPrev { false };

    bool pressed = isMouseButtonPressed(btn);
    bool res = pressed && !m_mouseButtonPressedPrev[btn];
    m_mouseButtonPressedPrev[btn] = pressed;
    return res;
}

bool GLContext::isMouseButtonReleased(int btn) const {
    return glfwGetMouseButton(m_window.ptr, btn) == GLFW_RELEASE;
}

glm::vec2 GLContext::getCursorPos() const {
    double x, y;
    glfwGetCursorPos(m_window.ptr, &x, &y);
    return { x - 0.5f*m_window.width, -y + 0.5f*m_window.height };
}

void GLContext::setCursorPos(glm::vec2 pos) const {
    glfwSetCursorPos(m_window.ptr, pos.x + 0.5f*m_window.width, -pos.y + 0.5f*m_window.height);
}

void GLContext::hideCursor() const {
    glfwSetInputMode(m_window.ptr, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void GLContext::showCursor() const {
    glfwSetInputMode(m_window.ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


/*
 * struct shrekrooms::gl::GLContext::Window
*/

glm::ivec2 GLContext::Window::getSizeVector() const {
    return { width, height };
}

float GLContext::Window::getAspectRatio() const {
    return static_cast<float>(width) / static_cast<float>(height);
}
