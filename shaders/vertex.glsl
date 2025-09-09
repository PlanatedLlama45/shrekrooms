#version 420 core

layout (location=0) in vec3 v_pos;
layout (location=1) in vec2 v_texCoord;
layout (location=2) in float v_texId;

out vec2 f_texCoord;
out float f_texId;
out vec3 f_pos;

uniform mat4 u_translate;
uniform mat4 u_rotate;
uniform mat4 u_view;
uniform mat4 u_projection;


void main() {
    mat4 moveMatrix = u_translate * u_rotate;
    mat4 fullMatrix = u_projection * u_view * moveMatrix;
    gl_Position = fullMatrix * vec4(v_pos, 1.0f);

    f_texCoord = v_texCoord;
    f_texId = v_texId;
    f_pos = (moveMatrix * vec4(v_pos, 1.0f)).xyz;
}
