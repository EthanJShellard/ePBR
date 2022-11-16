#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace ePBR 
{
	void Texture::Load(std::string _fileName, int _streams) 
	{
		// If we've already loaded a texture, unload it first
		if (m_ID) 
		{
			glDeleteTextures(1, &m_ID);
		}

		// Load SDL surface
		int width, height;
		stbi_uc* data = stbi_load(_fileName.c_str(), &width, &height, NULL, _streams);

		if (!data) // Check it worked
		{
			throw std::runtime_error("WARNING: could not load texture: " + _fileName);
		}

		// Create OpenGL texture
		glGenTextures(1, &m_ID);

		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// By default, OpenGL mag filter is linear
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// By default, OpenGL min filter will use mipmaps
		// We therefore either need to tell it to use linear or generate a mipmap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// SDL loads images in BGR order
		// Will need to handle different formats!!
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		//glGenerateMipmap(GL_TEXTURE_2D);

		free(data);
	}

	const GLuint Texture::GetID() const 
	{
		return m_ID;
	}

	Texture::Texture(std::string _fileName) 
	{
		Load(_fileName);
	}

	Texture::Texture() : 
		m_ID(0)
	{
	
	}

	Texture::~Texture() 
	{
		glDeleteTextures(1, &m_ID);
	}
}