#version 430 core
// This is the fragment shader
// The program in this file will be run separately for each fragment (pixel) that is drawn

// These are the per-fragment inputs
// They must match with the outputs of the vertex shader
in vec3 normalV;
in vec3 positionV;
in vec2 texCoordV;
in mat3 TBN;

// These variables will be the same for every vertex in the model
// They are mostly material and light properties
// We provide default values in case the program doesn't set them
uniform vec3 lightColour = {1,1,1};
uniform vec3 lightPos = {1.0,0.0,2.0};
uniform vec3 emissive = {0,0,0};
uniform vec3 ambient  = {0.1f,0.1f,0.2f};
uniform vec3 albedo  = {0.8f,0.1f,0.1f};
uniform vec3 specularColour = {1.0f,1.0f,1.0f};
uniform float shininess     = 50.0f;
uniform float alpha         = 1.0f;
uniform vec3 camPos;


// This is another input to allow us to access a texture
layout(location = 0) uniform sampler2D albedoMap;
layout(location = 1) uniform sampler2D normalMap;

// This is the output, it is the fragment's (pixel's) colour
out vec4 fragColour;

// The actual program, which will run on the graphics card
void main()
{
	// This is the direction from the fragment to the light, in eye-space
	vec3 lightDir = normalize(lightPos - positionV);
	vec3 eyeDir = normalize(camPos - positionV);
	vec3 halfVec = normalize(eyeDir + lightDir);

	// Sample the normal
	vec3 normal = vec3(texture(normalMap, vec2(texCoordV.x, texCoordV.y)));
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);
	
	// Retrieve colour from texture
	vec3 texCol = vec3(texture(albedoMap,vec2(texCoordV.x,1-texCoordV.y)));

	// Lambertian diffuse
	vec3 diffuse = texCol * (max(0,dot(normal,lightDir)));
		
	// Blinn-Phong specular
    vec3 specular = specularColour * pow(max(dot(halfVec, normal), 0.0), shininess);
		
	// The final output colour is the emissive + ambient + diffuse + specular
	// Removed emissive from output as it is breaking everything
	fragColour = vec4( emissive + ambient + diffuse + specular, alpha);
}
