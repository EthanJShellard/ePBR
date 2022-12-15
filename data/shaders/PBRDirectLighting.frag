#version 430 core

// TESTING BRDF with point lighting. IBL to come.

// These are the per-fragment inputs
// They must match with the outputs of the vertex shader
in vec3 positionV;
in vec3 normalV;
in vec2 texCoordV;
in mat3 TBN;

// Uniforms
uniform vec3 camPos;

//Test uniforms, constant properties
uniform vec3 albedo;
uniform float metalness;
uniform float roughness;
uniform float ambientOcclusion;

// This is another input to allow us to access a texture
layout(location = 0) uniform sampler2D albedoMap;
layout(location = 1) uniform sampler2D normalMap;
layout(location = 2) uniform sampler2D metalnessMap;
layout(location = 3) uniform sampler2D roughnessMap;
layout(location = 4) uniform sampler2D ambientOcclusionMap;
layout(location = 5) uniform samplerCube irradianceMap;
layout(location = 6) uniform samplerCube prefilterMap;
layout(location = 7) uniform sampler2D brdfLUT;

// This is the output, it is the fragment's (pixel's) colour
out vec4 fragColour;

// Define PI
const float PI = 3.14159265359;
const float e = 2.71828;

// https://learnopengl.com/PBR/Lighting
// Calculate the ratio between specular and diffuse reflection.
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// https://learnopengl.com/PBR/Lighting
// Fresnel schlick but with injected roughness. To be used when sampling an irradiance map as we will have no one halfway vector.
// Uses technique described by Sebastien Lagarde.
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// https://learnopengl.com/PBR/Lighting
// DistributionGGX (NDF)
float DistributionGGX(vec3 normal, vec3 halfVec, float roughness)
{
    float a = roughness * roughness;
    float a2 = a*a;
    float nDotH = max(dot(normal, halfVec), 0.0);
    float nDotH2 = nDotH * nDotH;

    float num = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Beckmann Distribution (NDF)
float BeckmannDistribution(float NDotH, float roughness)
{
    float r2 = roughness * roughness;

    float denom = (PI * r2 * pow(NDotH, 4.0));
    float numerator = pow(e, (NDotH * NDotH - 1.0) / (r2 * NDotH * NDotH));
    return (numerator / denom);
}

// https://learnopengl.com/PBR/Lighting
// GeometrySchlickGGX
float GeometrySchlickGGX(float nDotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = nDotV;
    float denom = nDotV * (1.0 - k) + k;

    return num / denom;
}

// https://learnopengl.com/PBR/Lighting
// GeometrySmith
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(nDotV, roughness);
    float ggx1 = GeometrySchlickGGX(nDotL, roughness);

    return ggx1 * ggx2;
}

void main()
{
    vec3 viewDir = normalize(camPos - positionV);

    // Sample albedo
    vec3 texAlbedo = vec3(texture(albedoMap,vec2(texCoordV.x,1-texCoordV.y)));

    //vec3 normal = normalize(normalV);
    // Use TBN to transform tangent space normals
    vec3 normal = vec3(texture(normalMap, vec2(texCoordV.x, texCoordV.y)));
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);

    // Sample metalness
    float texMetalness = texture(metalnessMap, vec2(texCoordV.x, texCoordV.y)).x;

    // Sample roughness
    float texRoughness = texture(roughnessMap, vec2(texCoordV.x, texCoordV.y)).x;

    // Need surface reflection at zero incidence (from directly above)
    // We approximate dielectrics to 0.04 and interpolate based on metalness.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, texAlbedo, texMetalness);

    // Test 4 set lights
    vec3 lightPositions[1] = vec3[1]
    (
        vec3(1, 0, 2.0)//,
        //vec3( 0, 0.5, 3.0),
        //vec3( 3.0, 0.5, 0),
        //vec3( 0, 0.5, -3.0)
    );
    vec3 lightColours[1] = vec3[1]
    (
        vec3( 1, 1, 1)//,
        //vec3( 1, 1, 1),
        //vec3( 1, 1, 1),
        //vec3( 1, 1, 1)
    );

    // Reflectance:
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; i++)
    {
        vec3 lightDir = normalize(lightPositions[i] - positionV);
        vec3 halfVec = normalize(viewDir + lightDir);

        float distance = length(lightPositions[i] - positionV);
        float attenuation = 5.0 / (distance * distance);
        vec3 radiance = lightColours[i] * attenuation;

        // Calculate fresnel
        vec3 F = fresnelSchlick(max(dot(halfVec, viewDir), 0.0), F0);

        // Calculate geometry occlusion
        float G = GeometrySmith(normal, viewDir, lightDir, texRoughness);
        // Calculate normal distribution
        float NDF = DistributionGGX(normal, halfVec, texRoughness);
        //float NDF = BeckmannDistribution(max(dot(normal, halfVec), 0.0), texRoughness);

        // Fresnel corrensponds to kS (the energy of light that gets reflected)
        vec3 kS = F;
        // Ratio of refraction (remaining after reflection)
        vec3 kD = vec3(1.0) - kS; 
        kD *= 1.0 - texMetalness;

        // Calculate Cook-Torrance BRDF
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // Calculate outgoing reflectance value
        float nDotL = max(dot(normal, lightDir), 0.0);
        Lo += (kD * texAlbedo / PI + specular) * radiance * nDotL;
    }

    // Fake ambient
    vec3 ambient = vec3(0.001) * texAlbedo/* * ao*/;

    // Final lit colour
    vec3 colour = ambient + Lo;

    // tone map and gamma correct to increase dynamic range using 
    // Reinhard operator seeing as we have no postprocessing for now
    colour = colour / (colour + vec3(1.0));
    colour = pow(colour, vec3(1.0/2.2));

    fragColour = vec4(colour, 1.0);
}