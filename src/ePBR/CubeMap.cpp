#include "CubeMap.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

#include <GL/glew.h>
#include <glm/ext.hpp>

namespace ePBR 
{
	GLuint CubeMap::GetMapID() const 
	{
		return m_mapID;
	}

	CubeMap::CubeMap() :
		m_frameBufferID(0),
		m_renderBufferID(0),
		m_mapID(0)
	{
	}

	CubeMap::CubeMap(int _width, std::shared_ptr<Texture> _equirectangularMap, std::shared_ptr<Shader> _equirectangularToCubemapShader) :
		m_frameBufferID(0),
		m_renderBufferID(0),
		m_mapID(0)
	{

		glGenFramebuffers(1, &m_frameBufferID);
		glGenRenderbuffers(1, &m_renderBufferID);

		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _width);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferID);

		// Generate textures
		glGenTextures(1, &m_mapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_mapID);

		for (unsigned int i = 0; i < 6; i++) 
		{
			// Presuming HDR for now
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, _width, _width, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 viewMatrices[] =
		{
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f))
		};

		GLuint mapLocation = glGetUniformLocation(_equirectangularToCubemapShader->GetID(), "equirectangularMap");
		GLuint projLocation = glGetUniformLocation(_equirectangularToCubemapShader->GetID(), "projection");

		glUseProgram(_equirectangularToCubemapShader->GetID());
		glUniform1i(mapLocation, 0);
		glUniformMatrix4fv(projLocation, 1, false, glm::value_ptr(projectionMat));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _equirectangularMap->GetID());

		glViewport(0,0, _width, _width);
		//glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);

		GLuint viewMatLoc = glGetUniformLocation(_equirectangularToCubemapShader->GetID(), "view");

		// Create cube mesh
		Mesh mesh;
		mesh.SetAsCube(0.5f);

		glFrontFace(GL_CW);

		for (unsigned int i = 0; i < 6; i++) 
		{
			glUniformMatrix4fv(viewMatLoc, 1, false, glm::value_ptr(viewMatrices[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_mapID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mesh.Draw();
		}

		glFrontFace(GL_CCW);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	CubeMap::~CubeMap() 
	{
		glDeleteTextures(1, &m_mapID);
		glDeleteRenderbuffers(1, &m_renderBufferID);
		glDeleteFramebuffers(1, &m_frameBufferID);
	}
}