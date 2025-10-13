#include "glc.hpp"
#include "player.hpp"
#include "shrek.hpp"


int main(int argc, const char **argv) {
    using namespace shrekrooms;
    srand(time(NULL));

    gl::GLContext glc { 640*2, 480*2, "Shrekrooms", false, GLFW_KEY_ESCAPE };
    UniformManager uniman = glc.getUniformManager();
    TextureManager texman { uniman };

    gl::Color bgcol { 0.2f, 0.2f, 0.2f };
    glc.setBackgroundColor(bgcol);

    maze::Maze maze { static_cast<size_t>(defines::world::chunksCountWidth), defines::world::bridgePercentage };
    MeshManager meshman { uniman, texman };
    World world { glc, meshman, maze };

    Player player { glc, { 0.0f, 0.0f, 0.0f }, 0.0f };
    Shrek shrek { glc, meshman, maze, { 5.0f, 0.0f, 5.0f } };

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

        // if (shrek.isCollidingPlayer(player))
        //     break;

        glc.enableShader();
        glc.clearBackground();

        shrek.draw(player);
        world.draw();

        glc.drawBuffer();
        deltaTime = glfwGetTime() - currentTime;
    }
    glc.showCursor();
    
    return 0;
}
