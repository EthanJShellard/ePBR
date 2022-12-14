#version 430 core
// This is the vertex shader
// The program in this file will be run separately for each vertex in the model

// This is the per-vertex input
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormalIn;
layout(location = 2) in vec2 vTexCoordIn;
layout(location = 3) in vec3 vTangentIn;
layout(location = 4) in vec3 vBitangentIn;

// These variables will be the same for every vertex in the model
uniform mat4 MVPMat;
uniform mat4 modelMat;

// These are the outputs from the vertex shader
// The data will (eventually) end up in the fragment shader
out vec2 texCoordV;
out vec3 normalV;
out vec3 positionV;
out mat3 TBN;

// The actual program, which will run on the graphics card
void main()
{
	// Viewing transformation
	// Incoming vertex position is multiplied by: modelling matrix, then viewing matrix, then projection matrix
	// gl_position is a special output variable
	gl_Position =  MVPMat * vec4(vPosition, 1);
	positionV =  vec3(modelMat * vec4(vPosition, 1));
	
	// Pass through the texture coordinate
	texCoordV = vTexCoordIn;
	
	// The surface normal is multiplied by the model and viewing matrices
	// This doesn't need to 'move' so we cast down to a 3x3 matrix
	normalV = mat3(modelMat) * vNormalIn;

	// Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(modelMat * vec4(vTangentIn, 0.0)));
    vec3 B = normalize(vec3(modelMat * vec4(vBitangentIn, 0.0)));
    vec3 N = normalize(vec3(modelMat * vec4(vNormalIn, 0.0)));
    TBN = mat3(T,B,N);
}
