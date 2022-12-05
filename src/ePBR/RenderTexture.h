#ifndef EPBR_RENDER_TEXTURE
#define EPBR_RENDER_TEXTURE

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <memory>

namespace ePBR 
{
	class Shader;
	class Mesh;

	class RenderTexture 
	{
		GLuint m_fbo;
		GLuint m_rbo;
		GLuint m_fbt;

		GLuint m_width;
		GLuint m_height;

	public:
		void Resize(unsigned int _width, unsigned int _height);

		/// @brief Bind this RenderTexture as framebuffer.
		void Bind();
		/// @brief Unbind this RenderTexture as framebuffer.
		void Unbind();
		GLuint GetTextureID();
		GLuint GetFBOID();

		GLuint GetWidth() const { return m_width; }
		GLuint GetHeight() const { return m_height; }

		RenderTexture(unsigned int _width, unsigned int _height);
		~RenderTexture();
	};
}

#endif // EPBR_RENDER_TEXTURE