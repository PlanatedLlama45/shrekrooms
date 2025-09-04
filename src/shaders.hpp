#pragma once

#include "imports.hpp"

namespace shrekrooms::shaders {


const char vertex[] =
    "#version 420 core                                                  \n"
    "                                                                   \n"
    "layout (location=0) in vec3 vertexPos;                             \n"
    "layout (location=1) in vec2 vertexTexCoord;                        \n"
    "layout (location=2) in float texId_v;                              \n"
    "                                                                   \n"
    "out vec2 fragTexCoord;                                             \n"
    "out float texId_f;                                                 \n"
    "out vec3 vertexPos_f;                                              \n"
    "                                                                   \n"
    "uniform mat4 translate;                                            \n"
    "uniform mat4 view;                                                 \n"
    "uniform mat4 projection;                                           \n"
    "                                                                   \n"
    "void main() {                                                      \n"
    "    gl_Position = projection*view*translate*vec4(vertexPos, 1.0f); \n"
    "                                                                   \n"
    "    fragTexCoord = vertexTexCoord;                                 \n"
    "    texId_f = texId_v;                                             \n"
    "    vertexPos_f = (translate*vec4(vertexPos, 1.0f)).xyz;           \n"
    "}                                                                  \n";

const char fragment[] =
    "#version 420 core                                                  \n"
    "                                                                   \n"
    "in vec2 fragTexCoord;                                              \n"
    "in float texId_f;                                                  \n"
    "in vec3 vertexPos_f;                                               \n"
    "                                                                   \n"
    "out vec4 screenColor;                                              \n"
    "                                                                   \n"
    "uniform vec4 color;                                                \n"
    "uniform vec4 fogColor;                                             \n"
    "uniform vec3 viewPos;                                              \n"
    "layout (binding=0) uniform sampler2D texture0;                     \n"
    "layout (binding=1) uniform sampler2D texture1;                     \n"
    "                                                                   \n"
    "                                                                   \n"
    "void main() {                                                      \n"
    "    if (texId_f == 0.0f)                                           \n"
    "        screenColor = texture(texture0, fragTexCoord);             \n"
    "    else if (texId_f == 1.0f)                                      \n"
    "        screenColor = texture(texture1, fragTexCoord);             \n"
    "    else                                                           \n"
    "        screenColor = color;                                       \n"
    "                                                                   \n"
    "    float dist = distance(vertexPos_f, viewPos);                   \n"
    "    const float fogNear = 10.0f;                                   \n"
    "    const float fogFar  = 20.0f;                                   \n"
    "                                                                   \n"
    "    if (screenColor.a == 0)                                        \n"
    "        discard;                                                   \n"
    "    float t = (dist - fogNear)/(fogFar - fogNear);                 \n"
    "    screenColor = mix(screenColor, fogColor, t);                   \n"
    "}                                                                  \n";


} // namespace shrekrooms::shaders
