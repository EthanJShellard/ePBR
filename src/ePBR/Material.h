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
		virtual void SetShader(std::shared_ptr<Shader> _newShader) = 0;

		virtual void Apply(glm::mat4 _modelMatrix, glm::mat4 _invModelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos) = 0;
	};
}

#endif // EPBR_MATERIAL