#version 330 core

out vec4 fragColour;
in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

// https://learnopengl.com/PBR/IBL/Diffuse-irradiance
// Spherical to cartesian trig magic
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

// https://learnopengl.com/PBR/IBL/Diffuse-irradiance
void main()
{
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 colour = texture(equirectangularMap, uv).rgb;

    fragColour = vec4(colour, 1.0);
}