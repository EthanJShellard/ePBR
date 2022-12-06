#version 330 core

out vec4 fragColour;

in vec3 localPos;

uniform samplerCube environmentMap;

void main()
{
    vec3 envColour = texture(environmentMap, localPos).rgb;

    // Tone mapping and gamma correction
    envColour = envColour / (envColour + vec3(1.0));
    envColour = pow(envColour, vec3(1.0/2.2));

    fragColour = vec4(envColour, 1.0);
}