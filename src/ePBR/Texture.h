#ifndef EPBR_TEXTURE
#define EPBR_TEXTURE

#include <string>

#include <GL/glew.h>

namespace ePBR 
{
	class Texture 
	{
		GLuint m_ID;

	public:
		void Load(std::string _fileName);
		void LoadHDR(std::string _fileName);
		const GLuint GetID() const;

		Texture(std::string _fileName, bool _isHDR = false);
		Texture();
		~Texture();
	};
}

#endif // EPBR_TEXTURE