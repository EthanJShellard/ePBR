#ifndef EPBR_SHADER
#define EPBR_SHADER

#include <GL/glew.h>
#include <string>

namespace ePBR
{
	/// @brief Shader wrapper with lazy compilation
	class Shader
	{
	public:
		/// @brief Load a new vertex shader for this shader program.
		/// @details Compilation and linking will be performed when GetID is called.
		/// @param _path The path to the new shader.
		void LoadNewVertexShader(const char* _path);

		/// @brief Load a new fragment shader for this shader program.
		/// @details Compilation and linking will be performed when GetID is called.
		/// @param _path The path to the new shader.
		void LoadNewFragmentShader(const char* _path);

		/// @brief Bind an OpenGL attribute at a specified index.
		/// @param index The index at which the attribute will be bound.
		/// @param _identifier The identifier of the attribute.
		void BindAttribute(int index, const char* _identifier);

		/// @brief Link shader program if anything has changed and return the ID
		/// @return The OpenGL ID of this shader program.
		GLuint GetID();

		/// @brief Create a shader program using a vertex and fragment shader.
		/// @param _vertexPath The path to the vertex shader.
		/// @param _fragmentPath The path to the fragment shader.
		Shader(const char* _vertexPath, const char* _fragmentPath);

		/// @brief Create a shader program using a vertex and fragment shader.
		/// @param _vertexPath The path to the vertex shader.
		/// @param _fragmentPath The path to the fragment shader.
		Shader(const std::string& _vertexPath, const std::string& _fragmentPath);

		/// @brief Create an empty shader.
		Shader();
		~Shader();
	protected:
		GLuint m_vertID;
		GLuint m_fragID;
		GLuint m_id;

		//If attributes or shaders are changed, program will need to be relinked.
		//Dirty is used to track if there have been changes since last link.
		bool m_dirty;
	};
}

#endif // EPBR_SHADER