#version 430 core

// Per-vertex inputs
layout(location = 0) in vec3 vPositionIn;
layout(location = 1) in vec3 vNormalIn;
layout(location = 2) in vec2 vTexCoordIn;
layout(location = 3) in vec3 vTangentIn;
layout(location = 4) in vec3 vBitangentIn;

// Uniforms
uniform mat4 MVPMat;
uniform mat4 modelMat;

// Vertex shader outputs
out vec2 texCoordV;
out vec3 normalV;
out vec3 positionV;
out mat3 TBN;

// BE WARY OF SPACES (EYE-SPACE VS WORLD SPACE)


void main()
{
    // Transform position
    gl_Position = MVPMat * vec4(vPositionIn, 1);

    // Pass through tex coords
    texCoordV = vTexCoordIn;

    // Interpolated normal
    positionV = vec3(modelMat * vec4(vPositionIn, 1));
    normalV = vec3(mat3(modelMat) * vNormalIn);

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(modelMat * vec4(vTangentIn, 0.0)));
    vec3 B = normalize(vec3(modelMat * vec4(vBitangentIn, 0.0)));
    vec3 N = normalize(vec3(modelMat * vec4(vNormalIn, 0.0)));
    TBN = mat3(T,B,N);
}