#ifndef EPBR_TEXTURE
#define EPBR_TEXTURE

#include <string>

#include <GL/glew.h>

namespace ePBR 
{
	class Texture 
	{
		friend class Context;

		GLuint m_ID;

	public:
		/// @brief Load an SDR texture from a file.
		/// @param _fileName The path to the file.
		void Load(std::string _fileName);

		/// @brief Load an HDR texture from a file.
		/// @param _fileName The path to the file.
		void LoadHDR(std::string _fileName);

		/// @brief Get the OpenGL ID of this texture.
		/// @return The ID.
		const GLuint GetID() const;

		/// @brief Create a texture using a file.
		/// @param _fileName The path to the file.
		/// @param _isHDR Whether the file HDR formatted.
		Texture(std::string _fileName, bool _isHDR = false);

		/// @brief Create an empty texture.
		Texture();
		~Texture();
	};
}

#endif // EPBR_TEXTURE