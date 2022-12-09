#version 330 core

out vec4 fragColour;
in vec3 localPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;


// Van Der Corput sequence.
float RadicalInverse_Vdc(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Hammersly sequence for low-discrepancy samples, based on the Quasi-Monte Carlo method.
vec2 Hammersly(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_Vdc(i));
}

// Orient and bias sample vector torwards specular lobe. Using GGX NDF as described by Epic Games.
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Spherical to cartesian
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = cos(phi) * sinTheta;
    H.z = cosTheta;

    // From tangent-space vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main()
{
    vec3 N = normalize(localPos);
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 4096u;
    float totalWeight = 0.0;
    vec3 prefilteredColour = vec3(0.0);
    for (uint i = 0u; i < SAMPLE_COUNT; i++)
    {
        vec2 Xi = Hammersly(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V,H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            prefilteredColour += texture(environmentMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColour = prefilteredColour / totalWeight;

    fragColour = vec4(prefilteredColour, 1.0);
}