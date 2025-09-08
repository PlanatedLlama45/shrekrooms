#version 420 core

in vec2 f_texCoord;
in float f_texId;
in vec3 f_pos;

out vec4 outScreenColor;

uniform vec4 u_color;
uniform vec4 u_fogColor;
uniform vec3 u_viewPos;

layout (binding=0) uniform sampler2D u_texture0;
layout (binding=1) uniform sampler2D u_texture1;


vec4 calcFog(vec4 color, float dist) {
    const float fogStart = 2.0f;
    const float fogDensity = 0.5f;

    if (dist <= fogStart)
        return color;

    float t = exp(-(dist - fogStart) * fogDensity);

    return mix(u_fogColor, color, 2*t - t*t);
}

void main() {
    if (f_texId == 0.0f)
        outScreenColor = texture(u_texture0, f_texCoord);
    else if (f_texId == 1.0f)
        outScreenColor = texture(u_texture1, f_texCoord);
    else
        outScreenColor = u_color;

    if (outScreenColor.a == 0)
        discard;

    float dist = distance(f_pos, u_viewPos);
    outScreenColor = calcFog(outScreenColor, dist);
}
