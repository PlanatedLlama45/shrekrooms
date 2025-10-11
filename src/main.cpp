#include "glc.hpp"
#include "player.hpp"
#include "world.hpp"


int main(int argc, const char **argv) {
    using namespace shrekrooms;
    srand(time(NULL));

    gl::GLContext glc { 640*2, 480*2, "Shrekrooms", false, GLFW_KEY_ESCAPE };
    UniformManager uniman = glc.getUniformManager();

    gl::Color bgcol { 0.2f, 0.2f, 0.2f };
    // gl::Color bgcol { 0.3f, 0.65f, 0.85f };
    glc.setBackgroundColor(bgcol);

    gl::Texture floorTex { "../img/floor.jpg" };
    gl::Texture wallTex { "../img/wall.jpg" };
    Player player { glc, { 0.0f, 0.0f, 0.0f }, 0.0f };
    maze::Maze maze { defines::world::chunksCountWidth, defines::world::bridgePercentage };
    MeshManager meshman { uniman, floorTex, wallTex };
    World world { glc, meshman, maze };

    glc.enableShader();
    uniman.setColor({ 1.0f, 0.0f, 1.0f });
    uniman.setFogColor(bgcol);

    float deltaTime = 0.001f;
    double currentTime;
    glc.setCursorPos({ 0.0f, 0.0f });
    glc.hideCursor();
    while (glc.isRunning()) {
        currentTime = glfwGetTime();

        glfwPollEvents();

        player.update(world, deltaTime);

        glc.enableShader();
        glc.clearBackground();

        world.draw();

        glc.drawBuffer();
        deltaTime = glfwGetTime() - currentTime;
    }
    glc.showCursor();
    
    return 0;
}
