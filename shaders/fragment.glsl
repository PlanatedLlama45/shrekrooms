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


const float DEG_TO_RAD = 0.01745329251f;

/*
 * x - 'distance' from falloff start
 * k - speed of falloff
*/
float exponentialFalloff(float x, float k) {
    float t = exp(-x * k);
    return (2*t - t*t);
}

vec4 calcFog(vec4 color, float dist) {
    const float FOG_START = 2.0f;
    const float FOG_DENSITY = 0.5f;

    if (dist <= FOG_START)
        return color;

    return mix(u_fogColor, color, exponentialFalloff(dist - FOG_START, FOG_DENSITY));
}

float calcBrightness() {
    const float LIGHT_INTENSITY = 20.0f;
    const float AMBIENT_LIGHT = 0.3f;

    const float LIGHT_MAX_ANGLE = 35.0f; // degrees
    const float LIGHT_MAX_ANGLE_COSINE = cos(LIGHT_MAX_ANGLE * DEG_TO_RAD);

    const float LIGHT_FALLOFF = 5.0f;

    vec3 normal;
    if (u_usePBR == 1) {
        normal = texture(u_texNormal, f_texCoord).rgb;
        normal = normalize(normal * 2.0f - 1.0f);
    } else
        normal = vec3(0.0f, 0.0f, 1.0f);
    normal = normalize(f_matTBN * normal); // transform: tangent space -> global space

    vec3 lightDir = u_viewPos - f_pos;
    float dist = length(lightDir);
    lightDir /= dist;

    float lambertian = max(dot(lightDir, normal), 0.0f);

    float brightness = LIGHT_INTENSITY * lambertian / (dist * dist);

    float angleCos = dot(lightDir, u_viewDir);
    if (angleCos < LIGHT_MAX_ANGLE_COSINE)
        brightness *= exponentialFalloff(angleCos - LIGHT_MAX_ANGLE_COSINE, LIGHT_FALLOFF);

    return clamp(brightness, AMBIENT_LIGHT, 1.0f);
}


void main() {
    outScreenColor = texture(u_texAlbedo, f_texCoord);

    if (outScreenColor.a == 0)
        discard;

    outScreenColor *= calcBrightness();

    float dist = distance(f_pos, u_viewPos);
    outScreenColor = calcFog(outScreenColor, dist);
}
