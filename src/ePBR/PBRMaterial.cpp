#include "PBRMaterial.h"
#include "Shader.h"

#include <fstream>
#include <iostream>

#include <SDL2/SDL.h>
#include <glm/gtc/type_ptr.hpp>

namespace ePBR 
{
	PBRMaterial::PBRMaterial() :
		m_albedo(glm::vec3(1)),
		m_metalness(0),
		m_roughness(0),
		m_albedoLocation(0),
		m_metalnessLocation(0),
		m_roughnessLocation(0),
		m_MVPMatLocation(0),
		m_modelMatLocation(0),
		m_camPosLocation(0),
		m_albedoTexture(0),
		m_normalMap(0),
		m_metalnessMap(0),
		m_albedoSamplerLocation(0),
		m_ambientOcclusionMapSamplerLocation(0),
		m_ambientOcclusionMap(0),
		m_metalnessMapSamplerLocation(0),
		m_normalMapSamplerLocation(0)
	{
	}

	PBRMaterial::~PBRMaterial() 
	{
		glDeleteTextures(1, &m_albedoTexture);
		glDeleteTextures(1, &m_normalMap);
		glDeleteTextures(1, &m_metalnessMap);
		glDeleteTextures(1, &m_ambientOcclusionMap);
	}

	bool PBRMaterial::LoadShaders(std::string _vertFilename, std::string _fragFilename)
	{
		// OpenGL doesn't provide any functions for loading shaders from file

		// The 'program' stores the shaders
		m_shaderProgram = std::make_shared<Shader>(_vertFilename.c_str(), _fragFilename.c_str());

		// Calling GetID will compile and link the newly created shader program
		GLint id = m_shaderProgram->GetID();
		
		glUseProgram(id);

		// Get uniform locations
		m_modelMatLocation = glGetUniformLocation(id, "modelMat");
		m_MVPMatLocation = glGetUniformLocation(id, "MVPMat");

		m_camPosLocation = glGetUniformLocation(id, "camPos");
		m_albedoLocation = glGetUniformLocation(id, "albedo");
		m_metalnessLocation = glGetUniformLocation(id, "metalness");
		m_roughnessLocation = glGetUniformLocation(id, "roughness");

		// Get texture sampler locations
		m_albedoSamplerLocation = glGetUniformLocation(id, "albedoMap");
		m_normalMapSamplerLocation = glGetUniformLocation(id, "normalMap");
		m_metalnessMapSamplerLocation = glGetUniformLocation(id, "metalnessMap");
		m_ambientOcclusionMapSamplerLocation = glGetUniformLocation(id, "ambientOcclusionMap");

		return true;
	}

	GLint PBRMaterial::LoadTexture(std::string _fileName) 
	{
		// Load SDL surface
		SDL_Surface* image = SDL_LoadBMP(_fileName.c_str());

		if (!image) // Check it worked
		{
			std::cerr << "WARNING: could not load BMP image: " << _fileName << std::endl;
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

	void PBRMaterial::Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos) 
	{
		glUseProgram(m_shaderProgram->GetID());

		// Calculate MVP
		glm::mat4 MVP = _projMatrix * _viewMatrix * _modelMatrix;

		// Upload matrices
		glUniformMatrix4fv(m_modelMatLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
		glUniformMatrix4fv(m_MVPMatLocation, 1, GL_FALSE, glm::value_ptr(MVP));

		glUniform3fv(m_camPosLocation, 1, glm::value_ptr(_camPos));
		glUniform3fv(m_albedoLocation, 1, glm::value_ptr(m_albedo));
		glUniform1f(m_metalnessLocation, m_metalness);
		glUniform1f(m_roughnessLocation, m_roughness);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_albedoTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normalMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_metalnessMap);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_ambientOcclusionMap);
	}
}