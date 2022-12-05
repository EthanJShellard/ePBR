
#ifndef EPBR_MATERIAL
#define EPBR_MATERIAL

#include <string>
#include <GLM/glm.hpp>
#include <GL/glew.h>

// Encapsulates shaders and textures
class Material
{
public:
	Material();
	~Material();

	// Loads shaders from file
	// Returns false if there was an error - it will also print out messages to console
	bool LoadShaders( std::string _vertFilename, std::string _fragFilename );

	// For setting the standard matrices needed by the shader
	void SetMatrices(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 projMatrix);
	
	// For setting material properties
	void SetEmissiveColour( glm::vec3 _input ) { m_emissiveColour = _input;}
	void SetDiffuseColour( glm::vec3 _input ) { m_diffuseColour = _input;}
	void SetSpecularColour( glm::vec3 _input ) { m_specularColour = _input;}

	// Set light position in world space
	void SetLightPosition( glm::vec3 _input ) { m_lightPosition = _input;}


	// Sets texture
	// This applies to ambient, diffuse and specular colours
	// If you want textures for anything else, you'll need to do that yourself ;)
	bool SetTexture( std::string _filename ) {m_texture1 = LoadTexture(_filename); return m_texture1>0;}

	// Sets the material, applying the shaders
	void Apply();

protected:

	// Utility function
	bool CheckShaderCompiled( GLint _shader );
	
	// The OpenGL shader program handle
	int m_shaderProgram;

	// Locations of Uniforms in the vertex shader
	int m_shaderModelMatLocation;
	int m_shaderInvModelMatLocation;
	int m_shaderViewMatLocation;
	int m_shaderProjMatLocation;

	// Location of Uniforms in the fragment shader
	int m_shaderDiffuseColLocation, m_shaderEmissiveColLocation, m_shaderSpecularColLocation;
	int m_shaderWSLightPosLocation;
	int m_shaderTex1SamplerLocation;

	// Local store of material properties to be sent to the shader
	glm::vec3 m_emissiveColour, m_diffuseColour, m_specularColour;
	glm::vec3 m_lightPosition;

	// Loads a .bmp from file
	unsigned int LoadTexture( std::string _filename );
	
	// OpenGL handle for the texture
	unsigned int m_texture1;

};

#endif // EPBR_MATERIAL
