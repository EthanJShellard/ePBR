#include <GL/glew.h>
#include <string>

namespace ePBR
{
	//Shader wrapper with lazy compilation
	class Shader
	{
	public:
		void LoadNewVertexShader(const char* _path);
		void LoadNewFragmentShader(const char* _path);
		void BindAttribute(int index, const char* _identifier);
		//Link shader program if anything has changed and return the ID
		GLuint GetID();

		Shader(const char* _vertexPath, const char* _fragmentPath);
		Shader(const std::string& _vertexPath, const std::string& _fragmentPath);
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