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
		void Bind() const;
		/// @brief Unbind this RenderTexture as framebuffer.
		void Unbind() const;

		/// @brief Get the OpenGL ID of this RenderTexture's texture.
		/// @return The ID.
		GLuint GetTextureID() const;

		/// @brief Get the OpenGL ID of the this RenderTexture's Framebuffer.
		/// @return The ID.
		GLuint GetFBOID() const;

		/// @brief Get the width of this RenderTexture.
		/// @return The width.
		GLuint GetWidth() const { return m_width; }

		/// @brief Get the height of this RenderTexture.
		/// @return The height.
		GLuint GetHeight() const { return m_height; }

		/// @brief Create a RenderTexture with specified width and height
		/// @param _width The width.
		/// @param _height The height.
		RenderTexture(unsigned int _width, unsigned int _height);
		~RenderTexture();
	};
}

#endif // EPBR_RENDER_TEXTURE