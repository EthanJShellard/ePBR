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

		void SetShader(std::shared_ptr<Shader> _newShader);

		void Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos);

		void SetAlbedoTexture(std::shared_ptr<Texture> _newTexture) { m_albedoTexture = _newTexture; }
		void SetNormalMap(std::shared_ptr<Texture> _newMap) { m_normalMap = _newMap; }

		std::shared_ptr<Texture> GetAlbedoTexture() { return m_albedoTexture; }
		std::shared_ptr<Texture> GetNormalMap() { return m_normalMap; }

		void SetAlbedo(glm::vec3 _newAlbedo) { m_albedo = _newAlbedo; }
		void SetAmbient(glm::vec3 _newAmbient) { m_ambient = _newAmbient; }
		void SetEmissive(glm::vec3 _newEmissive) { m_emissive = _newEmissive; }
		void SetShininess(float _newShininess) { m_shininess = _newShininess; }
		void SetAlpha(float _newAlpha) { m_alpha = _newAlpha; }

		glm::vec3 GetAlbedo() { return m_albedo; }
		glm::vec3 GetAmbient() { return m_ambient; }
		glm::vec3 GetEmissive() { return m_emissive; }
		float GetShininess() { return m_shininess; }
		float GetAlpha() { return m_alpha; }
	};
}

#endif // EPBR_LEGACY_MATERIAL