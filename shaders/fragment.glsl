#version 420 core

in vec2 f_texCoord;
in vec3 f_pos;
in mat3 f_matTBN;

out vec4 outScreenColor;

uniform vec4 u_color;
uniform vec4 u_fogColor;
uniform vec3 u_viewPos;
uniform vec3 u_viewDir;

uniform int u_usePBR;
layout (binding=0) uniform sampler2D u_texAlbedo;
layout (binding=1) uniform sampler2D u_texNormal;


vec4 calcFog(vec4 color, float dist) {
    const float fogStart = 2.0f;
    const float fogDensity = 0.5f;

    if (dist <= fogStart)
        return color;

    float t = exp(-(dist - fogStart) * fogDensity);

    return mix(u_fogColor, color, 2*t - t*t);
}

const float LIGHT_INTENSITY = 20.0f;
const float AMBIENT_LIGHT = 0.3f;

const float LIGHT_MAX_ANGLE = 35.0f; // degrees
const float LIGHT_MAX_ANGLE_COSINE = cos(LIGHT_MAX_ANGLE * 0.01745329251f);

float calcCoeff() {
    vec3 normal = texture(u_texNormal, f_texCoord).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    normal = normalize(f_matTBN * normal);

    vec3 lightDir = normalize(u_viewPos - f_pos);

    float dist = distance(u_viewPos, f_pos);

    float lambertian = max(dot(lightDir, normal), 0.0f);

    float coeff = clamp(LIGHT_INTENSITY * lambertian / (dist * dist), AMBIENT_LIGHT, 1.0f);
    float d = dot(lightDir, u_viewDir);
    if (d >= LIGHT_MAX_ANGLE_COSINE)
        return coeff;
    float t = exp(-(d - LIGHT_MAX_ANGLE_COSINE) * 5.0f);
    return clamp((2*t - t*t) * coeff, AMBIENT_LIGHT, 1.0f);
}

void main() {
    outScreenColor = texture(u_texAlbedo, f_texCoord);
    // outScreenColor = vec4(f_normal, 1.0f);

    if (outScreenColor.a == 0)
        discard;

    if (u_usePBR == 1) {
        // outScreenColor *= texture(u_texNormal, f_texCoord).r;
        // outScreenColor = texture(u_texNormal, f_texCoord);
        // vec3 normal = texture(u_texNormal, f_texCoord).rgb;
        // normal = normalize(normal * 2.0f - 1.0f)
    }

    outScreenColor *= calcCoeff();

    float dist = distance(f_pos, u_viewPos);
    outScreenColor = calcFog(outScreenColor, dist);
}
