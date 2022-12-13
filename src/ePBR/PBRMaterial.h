#ifndef EPBR_PBR_MATERIAL
#define EPBR_PBR_MATERIAL

#include "Texture.h"
#include "Material.h"

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace ePBR 
{
	// namepsaced forward declarations
	class Shader;
	class CubeMap;

	class PBRMaterial : public Material
	{
	public:
		PBRMaterial();
		~PBRMaterial();

		/// @brief Set this material's shader.
		/// @param _newShader The new shader.
		void SetShader(std::shared_ptr<Shader> _newShader);

		/// @brief Load a new shader program and apply it to this material.
		/// @param _vertFilename The path to the new vertex shader.
		/// @param _fragFilename The path to the new fragment shader.
		/// @return The newly crated shader program.
		std::shared_ptr<Shader> LoadShaderProgram(std::string _vertFilename, std::string _fragFilename);

		/// @brief Set the albedo modifier of this material.
		/// @param _input The new albedo modifier.
		void SetAlbedo(glm::vec3 _input) { m_albedo = _input; }

		/// @brief Set the roughness modifier of this material.
		/// @param _input The new roughness modifier.
		void SetRoughness(float _input) { m_roughness = _input; }

		/// @brief Set the metalness modifier of this material.
		/// @param _input The new metalness modifier.
		void SetMetalness(float _input) { m_metalness = _input; }

		/// @brief Get the albedo modifier of this material.
		/// @return The albedo modifier.
		glm::vec3 GetAlbedo() const { return m_albedo; }

		/// @brief Get the roughness modifier of this material.
		/// @return The roughness modifier.
		float GetRoughness() const { return m_roughness; }

		/// @brief Get the metalness modifier of this material.
		/// @return The metalness modifier.
		float GetMetalness() const { return m_metalness; }

		/// @brief Set the albedo texture of this material, loading it from a file and returning a pointer the new texture.
		/// @param _fileName The path to the abledo texture to load.
		/// @param _isHDR Whether or not the texture to load is HDR formatted.
		/// @return The newly loaded texture.
		std::shared_ptr<Texture> SetAlbedoTexture(std::string _fileName, bool _isHDR = false);
		
		/// @brief Set the normal map texture of this materiall, loading it from a file and returning a pointer the new texture.
		/// @param _fileName The path to the normal map texture to load.
		/// @param _isHDR Whether or not the texture to load is HDR formatted.
		/// @return The newly loaded texture.
		std::shared_ptr<Texture> SetNormalMap(std::string _fileName, bool _isHDR = false);
		
		/// @brief Set the metalness map texture of this material, loading it from a file and returning a pointer the new texture.
		/// @param _fileName The path to the metalness map texture to load.
		/// @param _isHDR Whether or not the texture to load is HDR formatted.
		/// @return The newly loaded texture.
		std::shared_ptr<Texture> SetMetalnessMap(std::string _fileName, bool _isHDR = false);
		
		/// @brief Set the roughness map texture of this material, loading it from a file and returning a pointer the new texture.
		/// @param _fileName The path to the roughness map texture to load.
		/// @param _isHDR Whether or not the texture to load is HDR formatted.
		/// @return The newly loaded texture.
		std::shared_ptr<Texture> SetRoughnessMap(std::string _fileName, bool _isHDR = false);
		
		/// @brief Set the Ambient Occlusion map texture of this material, loading it from a file and returning a pointer the new texture.
		/// @param _fileName The path to the Ambient Occlusion map texture to load.
		/// @param _isHDR Whether or not the texture to load is HDR formatted.
		/// @return The newly loaded texture.
		std::shared_ptr<Texture> SetAmbientOcclusionMap(std::string _fileName, bool _isHDR = false);

		/// @brief Set the albedo texture of this material.
		/// @param _newTex The new texture.
		void SetAlbedoTexture(std::shared_ptr<Texture> _newTex) { m_albedoTexture = _newTex; }

		/// @brief Set the normal map texture of this material.
		/// @param _newTex The new texture.
		void SetNormalMap(std::shared_ptr<Texture> _newTex) { m_normalMap = _newTex; }

		/// @brief Set the metalness map texture of this material.
		/// @param _newTex The new texture.
		void SetMetalnessMap(std::shared_ptr<Texture> _newTex) { m_metalnessMap = _newTex; }

		/// @brief Set the roughness map texture of this material.
		/// @param _newTex The new texture.
		void SetRoughnessMap(std::shared_ptr<Texture> _newTex) { m_roughnessMap = _newTex; }

		/// @brief Set the ambient occlusion map texture of this material.
		/// @param _newTex The new texture.
		void SetAmbientOcclusionMap(std::shared_ptr<Texture> _newTex) { m_ambientOcclusionMap = _newTex; }

		/// @brief Set the irradiance CubeMap of this material.
		/// @param _newMap The new CubeMap.
		void SetIrradianceMap(std::shared_ptr<CubeMap> _newMap) { m_irradianceMap = _newMap; }

		/// @brief Set the prefilter environment CubeMap of this material.
		/// @param _newMap The new CubeMap.
		void SetPrefilterEnvironmentMap(std::shared_ptr<CubeMap> _newMap) { m_prefilterMap = _newMap; }

		/// @brief Set the BRDF lookup texture of this material.
		/// @param _newLUT The new texture.
		void SetBRDFLookupTexture(std::shared_ptr<Texture> _newLUT) { m_brdfLUT = _newLUT; }

		/// @brief Apply this material in preparation for drawing something it applies to.
		/// @param _modelMatrix The model matrix of the object to draw.
		/// @param _invModelMatrix The inverse model matrix of the object to draw.
		/// @param _viewMatrix The view matrix which should be used to draw.
		/// @param _projMatrix The projection matrix which should be used to draw.
		/// @param _camPos The position of the camera corresponds to the view matrix.
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
		GLuint m_irradianceMapSamplerLocation;
		GLuint m_prefilteredEnvironmentMapSamplerLocation;
		GLuint m_brdfLookupTextureSamplerLocation;

		// PBR modifiers
		glm::vec3 m_albedo;
		float m_roughness;
		float m_metalness;

		// Textures
		std::shared_ptr<Texture> m_albedoTexture;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_metalnessMap;
		std::shared_ptr<Texture> m_roughnessMap;
		std::shared_ptr<Texture> m_ambientOcclusionMap;
		std::shared_ptr<CubeMap> m_irradianceMap;
		std::shared_ptr<CubeMap> m_prefilterMap;
		std::shared_ptr<Texture> m_brdfLUT;
	};
}

#endif // EPBR_PBR_MATERIAL