
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "Material.h"


Material::Material()
{
	// Initialise everything here
	m_shaderModelMatLocation = 0;
	m_shaderViewMatLocation = 0;
	m_shaderProjMatLocation = 0;

	m_shaderDiffuseColLocation = 0;
	m_shaderEmissiveColLocation = 0;
	m_shaderWSLightPosLocation = 0;
	m_shaderSpecularColLocation = 0;

	m_shaderTex1SamplerLocation = 0;

	m_texture1 = 0;
}

Material::~Material()
{
	// Clean up everything here
}


bool Material::LoadShaders( std::string _vertFilename, std::string _fragFilename )
{
	// OpenGL doesn't provide any functions for loading shaders from file

	
	std::ifstream vertFile( _vertFilename );
	char *vShaderText = NULL;

	if( vertFile.is_open() )
	{
		// Find out how many characters are in the file
		vertFile.seekg (0, vertFile.end);
		int length = (int) vertFile.tellg();
		vertFile.seekg (0, vertFile.beg);
		
		// Create our buffer
		vShaderText = new char [length + 1];

		// Transfer data from file to buffer
		vertFile.read(vShaderText,length);

		// Check it reached the end of the file
		if( !vertFile.eof() )
		{
			vertFile.close();
			std::cerr<<"WARNING: could not read vertex shader from file: "<<_vertFilename<<std::endl;
			return false;
		}

		// Find out how many characters were actually read
		length = (int) vertFile.gcount();

		// Needs to be NULL-terminated
		vShaderText[length] = 0;
		
		vertFile.close();
	}
	else
	{
		std::cerr<<"WARNING: could not open vertex shader from file: "<<_vertFilename<<std::endl;
		return false;
	}

	
	std::ifstream fragFile( _fragFilename );
	char *fShaderText = NULL;

	if( fragFile.is_open() )
	{
		// Find out how many characters are in the file
		fragFile.seekg (0, fragFile.end);
		int length = (int) fragFile.tellg();
		fragFile.seekg (0, fragFile.beg);
		
		// Create our buffer
		fShaderText = new char [length + 1];
		
		// Transfer data from file to buffer
		fragFile.read(fShaderText,length);
		
		// Check it reached the end of the file
		if( !fragFile.eof() )
		{
			fragFile.close();
			std::cerr<<"WARNING: could not read fragment shader from file: "<<_fragFilename<<std::endl;
			return false;
		}
		
		// Find out how many characters were actually read
		length = (int) fragFile.gcount();
		
		// Needs to be NULL-terminated
		fShaderText[length] = 0;
		
		fragFile.close();
	}
	else
	{
		std::cerr<<"WARNING: could not open fragment shader from file: "<<_fragFilename<<std::endl;
		return false;
	}



	// The 'program' stores the shaders
	m_shaderProgram = glCreateProgram();

	// Create the vertex shader
	GLuint vShader = glCreateShader( GL_VERTEX_SHADER );
	// Give GL the source for it
	glShaderSource( vShader, 1, &vShaderText, NULL );
	// Delete buffer
	delete [] vShaderText;
	// Compile the shader
	glCompileShader( vShader );
	// Check it compiled and give useful output if it didn't work!
	if( !CheckShaderCompiled( vShader ) )
	{
		std::cerr<<"ERROR: failed to compile vertex shader"<<std::endl;
		return false;
	}
	// This links the shader to the program
	glAttachShader( m_shaderProgram, vShader );

	// Same for the fragment shader
	GLuint fShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fShader, 1, &fShaderText, NULL );
	// Delete buffer
	delete [] fShaderText;
	glCompileShader( fShader );
	if( !CheckShaderCompiled( fShader ) )
	{
		std::cerr<<"ERROR: failed to compile fragment shader"<<std::endl;
		return false;
	}
	glAttachShader( m_shaderProgram, fShader );

	// This makes sure the vertex and fragment shaders connect together
	glLinkProgram( m_shaderProgram );
	// Check this worked
	GLint linked;
	glGetProgramiv( m_shaderProgram, GL_LINK_STATUS, &linked );
	if ( !linked )
	{
		GLsizei len;
		glGetProgramiv( m_shaderProgram, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = new GLchar[len+1];
		glGetProgramInfoLog( m_shaderProgram, len, &len, log );
		std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
		delete [] log;

		return false;
	}


	// We will define matrices which we will send to the shader
	// To do this we need to retrieve the locations of the shader's matrix uniform variables
	glUseProgram( m_shaderProgram );
	m_shaderModelMatLocation = glGetUniformLocation( m_shaderProgram, "modelMat" );
	m_shaderInvModelMatLocation = glGetUniformLocation( m_shaderProgram, "invModelMat" );
	m_shaderViewMatLocation = glGetUniformLocation( m_shaderProgram, "viewMat" );
	m_shaderProjMatLocation = glGetUniformLocation( m_shaderProgram, "projMat" );
		
	m_shaderDiffuseColLocation = glGetUniformLocation( m_shaderProgram, "diffuseColour" );
	m_shaderEmissiveColLocation = glGetUniformLocation( m_shaderProgram, "emissiveColour" );
	m_shaderSpecularColLocation = glGetUniformLocation( m_shaderProgram, "specularColour" );
	m_shaderWSLightPosLocation = glGetUniformLocation( m_shaderProgram, "worldSpaceLightPos" );

	m_shaderTex1SamplerLocation = glGetUniformLocation( m_shaderProgram, "tex1" );

	return true;
}

bool Material::CheckShaderCompiled( GLint _shader )
{
	GLint compiled;
	glGetShaderiv( _shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled )
	{
		GLsizei len;
		glGetShaderiv( _shader, GL_INFO_LOG_LENGTH, &len );

		// OpenGL will store an error message as a string that we can retrieve and print
		GLchar* log = new GLchar[len+1];
		glGetShaderInfoLog( _shader, len, &len, log );
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete [] log;

		return false;
	}
	return true;
}


unsigned int Material::LoadTexture( std::string _filename )
{
	// Load SDL surface
	SDL_Surface *image = SDL_LoadBMP( _filename.c_str() );

	if( !image ) // Check it worked
	{
		std::cerr<<"WARNING: could not load BMP image: "<<_filename<<std::endl;
		return 0;
	}

	// Create OpenGL texture
	unsigned int texName = 0;
	glGenTextures(1, &texName);


	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// By default, OpenGL mag filter is linear
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// By default, OpenGL min filter will use mipmaps
	// We therefore either need to tell it to use linear or generate a mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// SDL loads images in BGR order
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_BGR, GL_UNSIGNED_BYTE, image->pixels);

	//glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(image);
	
	//glBindTexture(GL_TEXTURE_2D, 0);


	return texName;
}




void Material::SetMatrices(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix)
{
	glUseProgram( m_shaderProgram );
		// Send matrices and uniforms
	glUniformMatrix4fv(m_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix) );
	glUniformMatrix4fv(m_shaderInvModelMatLocation, 1, GL_TRUE, glm::value_ptr(_invModelMatrix) );
	glUniformMatrix4fv(m_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(_viewMatrix) );
	glUniformMatrix4fv(m_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(_projMatrix) );
}
	

void Material::Apply()
{
	glUseProgram( m_shaderProgram );

	glUniform4fv( m_shaderWSLightPosLocation, 1, glm::value_ptr(m_lightPosition) );
	
	glUniform3fv( m_shaderEmissiveColLocation, 1, glm::value_ptr(m_emissiveColour) );
	glUniform3fv( m_shaderDiffuseColLocation, 1, glm::value_ptr(m_diffuseColour) );
	glUniform3fv( m_shaderSpecularColLocation, 1, glm::value_ptr(m_specularColour) );
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_shaderTex1SamplerLocation,0);
	glBindTexture(GL_TEXTURE_2D, m_texture1);
}
