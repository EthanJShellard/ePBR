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

// Calculate the ratio between specular and diffuse reflection.
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel schlick but with injected roughness. To be used when sampling an irradiance map as we will have no one halfway vector.
// Uses technique described by Sebastien Lagarde.
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

    // Get prefiltered reflection colour
    vec3 R = reflect(-viewDir, normal);
    const float MAX_REFLECTION_LOD = 4.0; // Using 5 mip levels
    vec3 prefilteredColour = textureLod(prefilterMap, R, texRoughness * MAX_REFLECTION_LOD).rgb;

    // Sample brdfLookup texture using material roughness and angle between normal and view
    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, texRoughness);
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), texRoughness)).rg;
    vec3 specular = prefilteredColour * (F * envBRDF.x + envBRDF.y);

    // Separate diffuse and specular component of irradiance map
    vec3 kS = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, texRoughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse = irradiance * texAlbedo;
    
    vec3 colour = (kD * diffuse + specular); //* ao;

    // tone map and gamma correct to increase dynamic range using 
    // Reinhard operator seeing as we have no postprocessing for now
    colour = colour / (colour + vec3(1.0));
    colour = pow(colour, vec3(1.0/2.2));

    fragColour = vec4(colour, 1.0);
}