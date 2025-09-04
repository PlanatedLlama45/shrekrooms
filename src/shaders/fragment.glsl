#version 420 core

in vec2 fragTexCoord;
in float texId_f;
in vec3 vertexPos_f;

out vec4 screenColor;

uniform vec4 color;
uniform vec4 fogColor;
uniform vec3 viewPos;

layout (binding=0) uniform sampler2D texture0;
layout (binding=1) uniform sampler2D texture1;


vec4 calcFog(vec4 color, float dist) {
    const float fogStart = 3.0f;
    const float fogDensity = 0.5f;

    if (dist <= fogStart)
        return color;

    float t = exp(-(dist - fogStart) * fogDensity);

    return mix(fogColor, color, 2*t - t*t);
}

void main() {
    if (texId_f == 0.0f)
        screenColor = texture(texture0, fragTexCoord);
    else if (texId_f == 1.0f)
        screenColor = texture(texture1, fragTexCoord);
    else
        screenColor = color;

    if (screenColor.a == 0)
        discard;

    float dist = distance(vertexPos_f, viewPos);
    screenColor = calcFog(screenColor, dist);
}
