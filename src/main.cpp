#include "gl.hpp"
#include "player.hpp"
#include "world.hpp"

int main(int argc, const char **argv) {
    using namespace shrekrooms;

    gl::GLContext glc { 640, 480, "Shrekrooms", false, GLFW_KEY_ESCAPE };
    gl::UniformManager uniman = glc.getUniformManager();

    gl::Color bgcol { 0.3f, 0.65f, 0.85f };

    gl::Texture floorTex { "../img/floor.jpg" };
    Player player { glc, { 0.0f, 0.0f, 0.0f }, 0.0f };
    World world { glc, floorTex };

    glc.enableShader();
    uniman.setColor({ 1.0f, 0.0f, 0.0f, 1.0f });

    glm::mat4 projMat = glm::perspective(gl::deg2rad*90.0f, glc.getWindow().getAspectRatio(), 0.1f, 50.0f);
    uniman.setProjectionMatrix(projMat);
    uniman.setTranslateMatrix(gl::mat4identity);

    float deltaTime = 0.001f;
    double currentTime;
    glc.setCursorPos({ 0.0f, 0.0f });
    glc.hideCursor();
    while (glc.isRunning()) {
        currentTime = glfwGetTime();

        glfwPollEvents();

        player.update(deltaTime);

        glc.enableShader();
        glc.clearBackground(bgcol);

        world.draw();

        glc.drawBuffer();
        deltaTime = glfwGetTime() - currentTime;
    }
    glc.showCursor();
    
    return 0;
}
