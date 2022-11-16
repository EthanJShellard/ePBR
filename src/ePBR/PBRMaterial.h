#ifndef EPBR_PBR_MATERIAL
#define EPBR_PBR_MATERIAL

#include "Texture.h"

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace ePBR 
{
	// namepsaced forward declarations
	class Shader;

	class PBRMaterial
	{
	public:
		PBRMaterial();
		~PBRMaterial();

		void SetShader(std::shared_ptr<Shader> _newShader) { m_shaderProgram = _newShader; };
		bool LoadShaders(std::string _vertFilename, std::string _fragFilename);

		void SetAlbedo(glm::vec3 _input) { m_albedo = _input; }
		void SetRoughness(float _input) { m_roughness = _input; }
		void SetMetalness(float _input) { m_metalness = _input; }
		glm::vec3 GetAlbedo() { return m_albedo; }
		float GetRoughness() { return m_roughness; }
		float GetMetalness() { return m_metalness; }

		bool SetAlbedoTexture(std::string _fileName) { m_albedoTexture->Load(_fileName); return m_albedoTexture != NULL; }
		bool SetNormalMap(std::string _fileName) { m_normalMap->Load(_fileName); return m_normalMap != NULL; }
		bool SetMetalnessMap(std::string _fileName) { m_metalnessMap->Load(_fileName); return m_metalnessMap != NULL; }
		bool SetRoughnessMap(std::string _fileName) { m_roughnessMap->Load(_fileName); return m_roughnessMap != NULL; }
		bool SetAmbientOcclusionMap(std::string _fileName) { m_ambientOcclusionMap->Load(_fileName); return m_ambientOcclusionMap != NULL; }

		void SetAlbedoTexture(std::shared_ptr<Texture> _newTex) { m_albedoTexture = _newTex; }
		void SetNormalMap(std::shared_ptr<Texture> _newTex) { m_normalMap = _newTex; }
		void SetMetalnessMap(std::shared_ptr<Texture> _newTex) { m_metalnessMap = _newTex; }
		void SetRoughnessMap(std::shared_ptr<Texture> _newTex) { m_roughnessMap = _newTex; }
		void SetAmbientOcclusionMap(std::shared_ptr<Texture> _newTex) { m_ambientOcclusionMap = _newTex; }

		void Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos);

	protected:

		std::shared_ptr<Shader> m_shaderProgram;

		// Vertex shader uniform locations
		GLuint m_MVPMatLocation;
		GLuint m_modelMatLocation;

		// Fragment shader uniform locations
		GLuint m_camPosLocation;
		GLuint m_albedoLocation;
		GLuint m_metalnessLocation;
		GLuint m_roughnessLocation;

		// Texture sampler locations
		GLuint m_albedoSamplerLocation;
		GLuint m_normalMapSamplerLocation;
		GLuint m_metalnessMapSamplerLocation;
		GLuint m_roughnessMapSamplerLocation;
		GLuint m_ambientOcclusionMapSamplerLocation;

		glm::vec3 m_albedo;
		float m_roughness;
		float m_metalness;

		// Switch to texture object when you get the chance
		std::shared_ptr<Texture> m_albedoTexture;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_metalnessMap;
		std::shared_ptr<Texture> m_roughnessMap;
		std::shared_ptr<Texture> m_ambientOcclusionMap;
	};
}

#endif // EPBR_PBR_MATERIAL