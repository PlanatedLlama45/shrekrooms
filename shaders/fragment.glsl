#version 420 core

in vec2 f_texCoord;
in vec3 f_pos;

out vec4 outScreenColor;

uniform vec4 u_color;
uniform vec4 u_fogColor;
uniform vec3 u_viewPos;

uniform sampler2D u_texture;


vec4 calcFog(vec4 color, float dist) {
    const float fogStart = 2.0f;
    const float fogDensity = 0.5f;

    if (dist <= fogStart)
        return color;

    float t = exp(-(dist - fogStart) * fogDensity);

    return mix(u_fogColor, color, 2*t - t*t);
}

void main() {
    outScreenColor = texture(u_texture, f_texCoord);

    if (outScreenColor.a == 0)
        discard;

    float dist = distance(f_pos, u_viewPos);
    outScreenColor = calcFog(outScreenColor, dist);
}
