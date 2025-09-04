#version 420 core

layout (location=0) in vec3 vertexPos;
layout (location=1) in vec2 vertexTexCoord;
layout (location=2) in float texId_v;

out vec2 fragTexCoord;
out float texId_f;
out vec3 vertexPos_f;

uniform mat4 translate;
uniform mat4 view;
uniform mat4 projection;


void main() {
    gl_Position = projection * view * translate * vec4(vertexPos, 1.0f);

    fragTexCoord = vertexTexCoord;
    texId_f = texId_v;
    vertexPos_f = (translate * vec4(vertexPos, 1.0f)).xyz;
}
