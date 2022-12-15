#ifndef EPBR_LEGACY_MATERIAL
#define EPBR_LEGACY_MATERIAL

#include "Material.h"

#include <GL/glew.h>

namespace ePBR 
{
	class Texture;

	class LegacyMaterial : public Material
	{
		// Uniform locations
		GLuint m_albedoLocation;
		GLuint m_ambientLocation;
		GLuint m_emissiveLocation;
		GLuint m_shininessLocation;
		GLuint m_alphaLocation;

		GLuint m_MVPLocation;
		GLuint m_modelMatLocation;
		GLuint m_camPosLocation;

		// Sampler locations
		GLuint m_albedoSamplerLocation;
		GLuint m_normalMapSamplerLocation;

		glm::vec3 m_albedo;
		glm::vec3 m_ambient;
		glm::vec3 m_emissive;
		float m_shininess;
		float m_alpha;

		std::shared_ptr<Texture> m_albedoTexture;
		std::shared_ptr<Texture> m_normalMap;

	public:
		LegacyMaterial();

		/// @brief Set this material's shader.
		/// @param _newShader The new shader.
		void SetShader(std::shared_ptr<Shader> _newShader);

		/// @brief Apply this material in preparation for drawing something it applies to.
		/// @param _modelMatrix The model matrix of the object to draw.
		/// @param _invModelMatrix The inverse model matrix of the object to draw.
		/// @param _viewMatrix The view matrix which should be used to draw.
		/// @param _projMatrix The projection matrix which should be used to draw.
		/// @param _camPos The position of the camera corresponds to the view matrix.
		void Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos);

		/// @brief Set the albedo texture of this material.
		/// @param _newTexture The new texture.
		void SetAlbedoTexture(std::shared_ptr<Texture> _newTexture) { m_albedoTexture = _newTexture; }

		/// @brief Set the normal map texture of this material.
		/// @param _newMap The new texture.
		void SetNormalMap(std::shared_ptr<Texture> _newMap) { m_normalMap = _newMap; }

		/// @brief Set the albedo modifier colour of this material.
		/// @param _newAlbedo The new albedo modifier.
		void SetAlbedo(glm::vec3 _newAlbedo) { m_albedo = _newAlbedo; }

		/// @brief Set the ambient lighting colour of this material.
		/// @param _newAmbient The new ambient lighting colour.
		void SetAmbient(glm::vec3 _newAmbient) { m_ambient = _newAmbient; }

		/// @brief Set the emssive lighting colour of this material.
		/// @param _newEmissive The new emissive lighting colour.
		void SetEmissive(glm::vec3 _newEmissive) { m_emissive = _newEmissive; }

		/// @brief Set the shininess of this material.
		/// @param _newShininess The new shininess.
		void SetShininess(float _newShininess) { m_shininess = _newShininess; }

		/// @brief Set the alpha value of this material.
		/// @param _newAlpha The new alpha value.
		void SetAlpha(float _newAlpha) { m_alpha = _newAlpha; }

		/// @brief Get the albedo modifier colour of this material.
		/// @return The abledo modifier colour.
		glm::vec3 GetAlbedo() { return m_albedo; }

		/// @brief Get the ambient lighting colour of this material.
		/// @return The ambient lighting colour.
		glm::vec3 GetAmbient() { return m_ambient; }

		/// @brief Get the emissive lighting colour of this material.
		/// @return The emissive lighting colour.
		glm::vec3 GetEmissive() { return m_emissive; }

		/// @brief Get the shininess of this material.
		/// @return The shininess.
		float GetShininess() { return m_shininess; }

		/// @brief Get the alpha value of this material.
		/// @return The alpha value.
		float GetAlpha() { return m_alpha; }
	};
}

#endif // EPBR_LEGACY_MATERIAL