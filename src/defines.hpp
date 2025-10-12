#pragma once

#include "imports.hpp"


#define _MAZE_DESMOS_OUTPUT 0


namespace shrekrooms::defines {


const glm::vec3 globalUp { 0.0f, 1.0f, 0.0f };
constexpr float deg2rad = M_PI / 180.0f;
const glm::mat4 mat4identity { 1.0f };
constexpr int quadVertCount = 6;
constexpr float epsilon = 1e-3f;


namespace shader {

    constexpr std::string_view vertex { "../shaders/vertex.glsl" };
    constexpr std::string_view fragment { "../shaders/fragment.glsl" };

}; // namespace shrekrooms::defines::shader


namespace world {

    constexpr float chunkSize = 5.0f;
    constexpr float chunkHeight = 5.0f;
    constexpr float wallThicknessHalf = 0.5f;
    constexpr int chunkFloorTiles = 2;
    constexpr int chunkWallTiles = 1;

    constexpr int chunksCountWidth = 10;
    constexpr float bridgePercentage = 0.2f;

}; // namespace shrekrooms::defines::world


namespace player {

    constexpr float mouseSensitivity    = 7.0f;
    constexpr float walkSpeed           = 8.0f;
    constexpr float radius              = 0.5f;

} // namespace shrekrooms::defines::player


namespace controls {

    constexpr int keyForward    = GLFW_KEY_W;
    constexpr int keyBackward   = GLFW_KEY_S;
    constexpr int keyLeft       = GLFW_KEY_A;
    constexpr int keyRight      = GLFW_KEY_D;

} // namespace shrekrooms::defines::controls


namespace shrek {

    constexpr float width       = 3.0f;
    constexpr float height      = 4.0f;
    constexpr float radius      = 1.5f;

    constexpr float walkSpeed   = 7.0f;

    constexpr float baseMoveTime = 6.0f;
    constexpr float moveTimeAmp  = 1.5f;
    constexpr float baseWaitTime = 1.0f;
    constexpr float waitTimeAmp  = 0.5f;

} // namespace shrekrooms::defines::shrek



}; // namespace shrekrooms::defines
