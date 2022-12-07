#version 330 core

out vec4 fragColour;
in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
    // The sample direction is our virtual hemisphere's orientation
    vec3 normal = normalize(localPos);

    vec3 irradiance = vec3(0.0);

    // Calculate directions
    vec3 up = vec3(0.0,1.0,0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float numSamples = 0.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
         for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
         {
             // Spherical to cartesian (tangent space)
             vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
             // Tangent space to world space
             vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

             irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
             numSamples++;
         }
    }

    irradiance = PI * irradiance * (1.0 / float(numSamples));
    fragColour = vec4(irradiance, 1.0);
}