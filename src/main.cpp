#include "glc.hpp"
#include "player.hpp"
#include "shrek.hpp"


int main(int argc, const char **argv) {
    using namespace shrekrooms;

    gl::GLContext glc { 640*2, 480*2, "Shrekrooms", false, GLFW_KEY_ESCAPE };
    rng::Random random;

    const UniformManager &uniman  = glc.getUniformManager();
    const TextureManager &texman  = glc.getUniformManager();
    const MeshManager    &meshman = glc.getMeshManager();

    gl::Color bgcol { 0.2f, 0.2f, 0.2f };
    glc.setBackgroundColor(bgcol);

    maze::Maze maze { random, defines::world::chunksCountWidth, defines::world::bridgePercentage };
    World world { glc, meshman, maze };

    Player player { glc, { 0.0f, 0.0f, 0.0f }, 0.0f };
    Shrek shrek { glc, meshman, maze, { 5, 5 } };

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

        shrek.draw(player);
        world.draw();

        glc.drawBuffer();
        deltaTime = glfwGetTime() - currentTime;
        // glfwSetWindowTitle(glc.getWindow().ptr, std::to_string(100 * static_cast<int>(0.01f / deltaTime)).c_str());
    }
    glc.showCursor();
    
    return 0;
}
