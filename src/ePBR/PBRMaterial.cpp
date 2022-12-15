#include "PBRMaterial.h"
#include "Shader.h"
#include "CubeMap.h"

#include <fstream>
#include <iostream>

#include <SDL2/SDL.h>
#include <glm/gtc/type_ptr.hpp>

namespace ePBR 
{
	PBRMaterial::PBRMaterial() :
		// Initialising GLuints to max value as 0 is a valid id/location - this makes for simpler debugging.
		m_albedo(glm::vec3(1)),
		m_metalness(0),
		m_roughness(0),
		m_albedoLocation(-1),
		m_metalnessLocation(-1),
		m_roughnessLocation(-1),
		m_MVPMatLocation(-1),
		m_modelMatLocation(-1),
		m_camPosLocation(-1),
		m_albedoSamplerLocation(-1),
		m_ambientOcclusionMapSamplerLocation(-1),
		m_metalnessMapSamplerLocation(-1),
		m_roughnessMapSamplerLocation(-1),
		m_normalMapSamplerLocation(-1),
		m_irradianceMapSamplerLocation(-1),
		m_prefilteredEnvironmentMapSamplerLocation(-1),
		m_brdfLookupTextureSamplerLocation(-1),
		m_shaderProgram(std::make_shared<Shader>()),
		m_albedoTexture(std::make_shared<Texture>()),
		m_normalMap(std::make_shared<Texture>()),
		m_metalnessMap(std::make_shared<Texture>()),
		m_ambientOcclusionMap(std::make_shared<Texture>()),
		m_roughnessMap(std::make_shared<Texture>())
	{
	}

	PBRMaterial::~PBRMaterial() 
	{
	}

	std::shared_ptr<Shader> PBRMaterial::LoadShaderProgram(std::string _vertFilename, std::string _fragFilename)
	{
		// OpenGL doesn't provide any functions for loading shaders from file

		// The 'program' stores the shaders
		m_shaderProgram->LoadNewVertexShader(_vertFilename.c_str());
		m_shaderProgram->LoadNewFragmentShader(_fragFilename.c_str());

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
		m_roughnessMapSamplerLocation = glGetUniformLocation(id, "roughnessMap");
		m_ambientOcclusionMapSamplerLocation = glGetUniformLocation(id, "ambientOcclusionMap");
		m_irradianceMapSamplerLocation = glGetUniformLocation(id, "irradianceMap");
		m_prefilteredEnvironmentMapSamplerLocation = glGetUniformLocation(id, "prefilterMap");
		m_brdfLookupTextureSamplerLocation = glGetUniformLocation(id, "brdfLUT");

		return m_shaderProgram;
	}

	void PBRMaterial::SetShader(std::shared_ptr<Shader> _newShader)
	{
		m_shaderProgram = _newShader;

		// Calling GetID will compile and link the newly created shader program
		GLuint id = m_shaderProgram->GetID();

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
		m_roughnessMapSamplerLocation = glGetUniformLocation(id, "roughnessMap");
		m_ambientOcclusionMapSamplerLocation = glGetUniformLocation(id, "ambientOcclusionMap");
		m_irradianceMapSamplerLocation = glGetUniformLocation(id, "irradianceMap");
		m_prefilteredEnvironmentMapSamplerLocation = glGetUniformLocation(id, "prefilterMap");
		m_brdfLookupTextureSamplerLocation = glGetUniformLocation(id, "brdfLUT");

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
		glUniform1i(m_albedoSamplerLocation, 0);
		glBindTexture(GL_TEXTURE_2D, m_albedoTexture->GetID());

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(m_normalMapSamplerLocation, 1);
		glBindTexture(GL_TEXTURE_2D, m_normalMap->GetID());

		glActiveTexture(GL_TEXTURE2);
		glUniform1i(m_metalnessMapSamplerLocation, 2);
		glBindTexture(GL_TEXTURE_2D, m_metalnessMap->GetID());

		glActiveTexture(GL_TEXTURE3);
		glUniform1i(m_roughnessMapSamplerLocation, 3);
		glBindTexture(GL_TEXTURE_2D, m_roughnessMap->GetID());

		glActiveTexture(GL_TEXTURE4);
		glUniform1i(m_ambientOcclusionMapSamplerLocation, 4);
		glBindTexture(GL_TEXTURE_2D, m_ambientOcclusionMap->GetID());

		if (m_irradianceMap) 
		{
			glActiveTexture(GL_TEXTURE5);
			glUniform1i(m_irradianceMapSamplerLocation, 5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap->GetMapID());
		}

		if (m_prefilterMap) 
		{
			glActiveTexture(GL_TEXTURE6);
			glUniform1i(m_prefilteredEnvironmentMapSamplerLocation, 6);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap->GetMapID());
		}

		if (m_brdfLUT) 
		{
			glActiveTexture(GL_TEXTURE7);
			glUniform1i(m_brdfLookupTextureSamplerLocation, 7);
			glBindTexture(GL_TEXTURE_2D, m_brdfLUT->GetID());
		}
	}

	std::shared_ptr<Texture> PBRMaterial::SetAlbedoTexture(std::string _fileName, bool _isHDR) 
	{
		_isHDR ? m_albedoTexture->LoadHDR(_fileName) : m_albedoTexture->Load(_fileName);
		return m_albedoTexture;
	}

	std::shared_ptr<Texture> PBRMaterial::SetNormalMap(std::string _fileName, bool _isHDR) 
	{
		_isHDR ? m_normalMap->LoadHDR(_fileName) : m_normalMap->Load(_fileName); 
		return m_normalMap;
	}

	std::shared_ptr<Texture> PBRMaterial::SetMetalnessMap(std::string _fileName, bool _isHDR) 
	{
		_isHDR ? m_metalnessMap->LoadHDR(_fileName) : m_metalnessMap->Load(_fileName); 
		return m_metalnessMap;
	}

	std::shared_ptr<Texture> PBRMaterial::SetRoughnessMap(std::string _fileName, bool _isHDR) 
	{
		_isHDR ? m_roughnessMap->LoadHDR(_fileName) : m_roughnessMap->Load(_fileName); 
		return m_roughnessMap;
	}

	std::shared_ptr<Texture> PBRMaterial::SetAmbientOcclusionMap(std::string _fileName, bool _isHDR) 
	{
		_isHDR ? m_ambientOcclusionMap->LoadHDR(_fileName) : m_ambientOcclusionMap->Load(_fileName); 
		return m_ambientOcclusionMap;
	}
}