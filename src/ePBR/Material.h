#ifndef EPBR_MATERIAL
#define EPBR_MATERIAL

#include <memory>
#include <glm/glm.hpp>

namespace ePBR 
{
	class Shader;

	class Material 
	{
	protected:
		std::shared_ptr<Shader> m_shaderProgram;

	public:
		/// @brief Set this material's shader.
		/// @param _newShader The new shader
		virtual void SetShader(std::shared_ptr<Shader> _newShader) = 0;

		/// @brief Apply this material in preparation for drawing something it applies to.
		/// @param _modelMatrix The model matrix of the object to draw.
		/// @param _invModelMatrix The inverse model matrix of the object to draw.
		/// @param _viewMatrix The view matrix which should be used to draw.
		/// @param _projMatrix The projection matrix which should be used to draw.
		/// @param _camPos The position of the camera corresponds to the view matrix.
		virtual void Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos) = 0;
	};
}

#endif // EPBR_MATERIAL