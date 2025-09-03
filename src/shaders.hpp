#pragma once

namespace shrekrooms::shaders {


const char vertex[] =
    "#version 330 core                                                  \n"
    "                                                                   \n"
    "layout (location=0) in vec3 vertexPos;                             \n"
    "layout (location=1) in vec2 vertexTexCoord;                        \n"
    "                                                                   \n"
    "out vec2 fragTexCoord;                                             \n"
    "                                                                   \n"
    "uniform mat4 translate;                                            \n"
    "uniform mat4 view;                                                 \n"
    "uniform mat4 projection;                                           \n"
    "                                                                   \n"
    "void main() {                                                      \n"
    "    gl_Position = projection*view*translate*vec4(vertexPos, 1.0f); \n"
    "                                                                   \n"
    "    fragTexCoord = vertexTexCoord;                                 \n"
    "}                                                                  \n";

const char fragment[] =
    "#version 330 core                                                  \n"
    "                                                                   \n"
    "in vec2 fragTexCoord;                                              \n"
    "                                                                   \n"
    "out vec4 screenColor;                                              \n"
    "                                                                   \n"
    "uniform sampler2D texture0;                                        \n"
    "uniform vec4 color;                                                \n"
    "uniform int useTexture;                                            \n"
    "uniform int renderText;                                            \n"
    "                                                                   \n"
    "void main() {                                                      \n"
    "    if (useTexture == 1 || renderText == 1) {                      \n"
    "        screenColor = texture(texture0, fragTexCoord);             \n"
    "        if (screenColor.a == 0)                                    \n"
    "            discard;                                               \n"
    "        if (renderText == 1)                                       \n"
    "            screenColor *= color;                                  \n"
    "    } else                                                         \n"
    "        screenColor = color;                                       \n"
    "}                                                                  \n";


} // namespace shrekrooms::shaders
