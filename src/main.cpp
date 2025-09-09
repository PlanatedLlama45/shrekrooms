#include "gl.hpp"
#include "world.hpp"
#include "player.hpp"
#include "shrek.hpp"


int main(int argc, const char **argv) {
    using namespace shrekrooms;

    initRandom();

    gl::GLContext glc { 640*2, 480*2, "Shrekrooms", false, GLFW_KEY_ESCAPE };
    gl::UniformManager uniman = glc.getUniformManager();

    gl::Color bgcol { 0.2f, 0.2f, 0.2f };
    // gl::Color bgcol { 0.3f, 0.65f, 0.85f };
    glc.setBackgroundColor(bgcol);

    gl::Texture floorTex { "../img/floor.jpg" };
    gl::Texture wallTex { "../img/wall.jpg" };
    gl::Texture shrekTex { "../img/shrek.jpg" };

    Player player { glc, { 0.0f, 0.0f, 4.0f }, 0.0f };
    Shrek shrek { glc, { 4.0f, 0.0f, 16.0f }, shrekTex };
    World world { glc, floorTex, wallTex };

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
        shrek.update(world, player, deltaTime);

        if (shrek.isCollidingPlayer(player))
            break;

        glc.enableShader();
        glc.clearBackground();

        world.draw();
        shrek.draw(player);

        glc.drawBuffer();
        deltaTime = glfwGetTime() - currentTime;
    }
    glc.showCursor();
    
    return 0;
}
