#include "RenderTexture.h"
#include "Mesh.h"
#include "Shader.h"

namespace ePBR 
{
	void RenderTexture::Resize(unsigned int _width, unsigned int _height)
	{
		m_width = _width;
		m_height = _height;

		//Refit texture
		glBindTexture(GL_TEXTURE_2D, m_fbt);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		//Refit renderBufferStorage
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void RenderTexture::Bind() const 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	}

	void RenderTexture::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint RenderTexture::GetTextureID() const
	{
		return m_fbt;
	}

	GLuint RenderTexture::GetFBOID() const
	{
		return m_fbo;
	}

	RenderTexture::RenderTexture(unsigned int _width, unsigned int _height)
	{
		m_width = _width;
		m_height = _height;

		//Create FrameBufferObject
		m_fbo = 0;
		glGenFramebuffers(1, &m_fbo);
		if (!m_fbo)
		{
			throw std::exception();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		//Create FrameBufferTexture
		m_fbt = 0;
		glGenTextures(1, &m_fbt);
		glBindTexture(GL_TEXTURE_2D, m_fbt);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		//Attach FrameBufferTexture to FrameBufferObject
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbt, 0);

		//Create RenderBufferObject
		m_rbo = 0;
		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		//Attach RenderBufferObject to FrameBufferObject
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	RenderTexture::~RenderTexture() 
	{
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteRenderbuffers(1, &m_rbo);
		glDeleteTextures(1, &m_fbt);
	}
}