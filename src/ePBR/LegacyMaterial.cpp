#include "LegacyMaterial.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/ext.hpp>

namespace ePBR 
{
	LegacyMaterial::LegacyMaterial() :
		m_albedo(1),
		m_ambient(0),
		m_emissive(0),
		m_shininess(1),
		m_alpha(1),
		m_albedoLocation(-1),
		m_ambientLocation(-1),
		m_emissiveLocation(-1),
		m_shininessLocation(-1),
		m_alphaLocation(-1),
		m_MVPLocation(-1),
		m_modelMatLocation(-1),
		m_albedoSamplerLocation(-1),
		m_normalMapSamplerLocation(-1)
	{
	}

	void LegacyMaterial::SetShader(std::shared_ptr<Shader> _newShader) 
	{
		m_shaderProgram = _newShader;

		GLuint id = m_shaderProgram->GetID();

		// Get uniform locations
		m_albedoLocation = glGetUniformLocation(id, "albedo");
		m_ambientLocation = glGetUniformLocation(id, "ambient");
		m_emissiveLocation = glGetUniformLocation(id, "emissive");
		m_shininessLocation = glGetUniformLocation(id, "shininess");
		m_alphaLocation = glGetUniformLocation(id, "alpha");

		m_MVPLocation = glGetUniformLocation(id, "MVPMat");
		m_modelMatLocation = glGetUniformLocation(id, "modelMat");
		m_camPosLocation = glGetUniformLocation(id, "camPos");

		// Sampler locations
		m_albedoSamplerLocation = glGetUniformLocation(id, "albedoMap");
		m_normalMapSamplerLocation = glGetUniformLocation(id, "normalMap");
	}

	void LegacyMaterial::Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos) 
	{
		glUseProgram(m_shaderProgram->GetID());

		// Calculate MVP
		glm::mat4 MVP = _projMatrix * _viewMatrix * _modelMatrix;

		// Upload matrices
		glUniformMatrix4fv(m_modelMatLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
		glUniformMatrix4fv(m_MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));

		glUniform3fv(m_camPosLocation, 1, glm::value_ptr(_camPos));
		glUniform3fv(m_albedoLocation, 1, glm::value_ptr(m_albedo));
		glUniform3fv(m_ambientLocation, 1, glm::value_ptr(m_ambient));
		glUniform3fv(m_emissiveLocation, 1, glm::value_ptr(m_emissive));
		glUniform1f(m_shininessLocation, m_shininess);
		glUniform1f(m_alphaLocation, m_alpha);

		if (m_albedoTexture) 
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(m_albedoSamplerLocation, 0);
			glBindTexture(GL_TEXTURE_2D, m_albedoTexture->GetID());
		}

		if (m_normalMap) 
		{
			glActiveTexture(GL_TEXTURE1);
			glUniform1i(m_normalMapSamplerLocation, 1);
			glBindTexture(GL_TEXTURE_2D, m_normalMap->GetID());
		}
	}
}