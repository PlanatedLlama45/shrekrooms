#include "gl.hpp"
#include "player.hpp"
#include "world.hpp"

int main(int argc, const char **argv) {
    using namespace shrekrooms;

    gl::GLContext glc { 640*2, 480*2, "Shrekrooms", false, GLFW_KEY_ESCAPE };
    gl::UniformManager uniman = glc.getUniformManager();

    gl::Color bgcol { 0.2f, 0.2f, 0.2f };
    // gl::Color bgcol { 0.3f, 0.65f, 0.85f };

    gl::Texture floorTex { "../img/floor.jpg" };
    gl::Texture wallTex { "../img/wall2.jpg" };
    Player player { glc, { -5.0f, 0.0f, -5.0f }, 0.0f };
    World world { glc, floorTex, wallTex };

    glc.enableShader();
    uniman.setColor({ 1.0f, 0.0f, 1.0f });
    uniman.setFogColor(bgcol);

    glm::mat4 projMat = glm::perspective(gl::deg2rad*90.0f, glc.getWindow().getAspectRatio(), 0.1f, 50.0f);
    uniman.setProjectionMatrix(projMat);
    uniman.setTranslateMatrix(gl::mat4identity);

    glEnable(GL_CULL_FACE);

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
