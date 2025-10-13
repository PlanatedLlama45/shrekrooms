#include "defines.hpp"


const glm::vec3 shrekrooms::defines::globalUp      { 0.0f, 1.0f, 0.0f };
const glm::mat4 shrekrooms::defines::mat4identity  { 1.0f };
const float     shrekrooms::defines::deg2rad       = M_PI / 180.0f;
const int       shrekrooms::defines::quadVertCount = 6;
const float     shrekrooms::defines::epsilon       = 1e-3f;


// namespace shrekrooms::defines::shader
const std::string_view shrekrooms::defines::shader::vertex   { "../shaders/vertex.glsl" };
const std::string_view shrekrooms::defines::shader::fragment { "../shaders/fragment.glsl" };

// namespace shrekrooms::defines::world
const float shrekrooms::defines::world::chunkSize         = 5.0f;
const float shrekrooms::defines::world::chunkHeight       = 5.0f;
const float shrekrooms::defines::world::wallThicknessHalf = 0.5f;
const int   shrekrooms::defines::world::chunkFloorTiles   = 2;
const int   shrekrooms::defines::world::chunkWallTiles    = 1;
const int   shrekrooms::defines::world::chunksCountWidth  = 10;
const float shrekrooms::defines::world::bridgePercentage  = 0.2f;

// namespace shrekrooms::defines::player
const float shrekrooms::defines::player::mouseSensitivity  = 7.0f;
const float shrekrooms::defines::player::walkSpeed         = 8.0f;
const float shrekrooms::defines::player::radius            = 0.5f;

// namespace shrekrooms::defines::controls
const int shrekrooms::defines::controls::keyForward  = GLFW_KEY_W;
const int shrekrooms::defines::controls::keyBackward = GLFW_KEY_S;
const int shrekrooms::defines::controls::keyLeft     = GLFW_KEY_A;
const int shrekrooms::defines::controls::keyRight    = GLFW_KEY_D;

// namespace shrekrooms::defines::shrek
const float shrekrooms::defines::shrek::width     = 3.0f;
const float shrekrooms::defines::shrek::height    = 4.0f;
const float shrekrooms::defines::shrek::radius    = 1.5f;
const float shrekrooms::defines::shrek::walkSpeed = 5.0f;
