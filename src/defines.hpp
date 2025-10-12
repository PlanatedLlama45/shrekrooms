#pragma once

#include "imports.hpp"


#define _MAZE_DESMOS_OUTPUT 0


namespace shrekrooms::defines {


extern const glm::vec3 globalUp;
extern const glm::mat4 mat4identity;
extern const float deg2rad;
extern const int quadVertCount;
extern const float epsilon;


namespace shader {

    extern const std::string_view vertex;
    extern const std::string_view fragment;

} // namespace shrekrooms::defines::shader


namespace world {

    extern const float chunkSize;
    extern const float chunkHeight;
    extern const float wallThicknessHalf;
    extern const int chunkFloorTiles;
    extern const int chunkWallTiles;
    extern const int chunksCountWidth;
    extern const float bridgePercentage;

} // namespace shrekrooms::defines::world


namespace player {

    extern const float mouseSensitivity;
    extern const float walkSpeed;
    extern const float radius;

} // namespace shrekrooms::defines::player


namespace controls {

    extern const int keyForward;
    extern const int keyBackward;
    extern const int keyLeft;
    extern const int keyRight;

} // namespace shrekrooms::defines::controls


namespace shrek {

    extern const float width;
    extern const float height;
    extern const float radius;
    extern const float walkSpeed;

} // namespace shrekrooms::defines::shrek



}; // namespace shrekrooms::defines
