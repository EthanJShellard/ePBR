#include "Renderer.h"

#include <GL/glew.h>

namespace ePBR
{
	Renderer::Renderer(int _width, int _height) :
		m_renderTexture(nullptr),
		m_model(nullptr),
		m_projectionMat(1.0f),
		m_viewMat(1.0f),
		m_modelMat(1.0f),
		m_camPos(0.0f),
		m_clearColour(0.0f,0.0f,0.0f,1.0f),
		m_depthTest(true),
		m_backfaceCull(false),
		m_blend(false),
		m_width(_width),
		m_height(_height)
	{
	}

	Renderer::Renderer(std::shared_ptr<RenderTexture> _renderTarget) :
		m_renderTexture(_renderTarget),
		m_model(nullptr),
		m_projectionMat(1.0f),
		m_viewMat(1.0f),
		m_modelMat(1.0f),
		m_camPos(0.0f),
		m_clearColour(0.0f, 0.0f, 0.0f, 1.0f),
		m_depthTest(true),
		m_backfaceCull(false),
		m_blend(false),
		m_width(_renderTarget->GetWidth()),
		m_height(_renderTarget->GetHeight())
	{
	}

	void Renderer::Draw() 
	{
		// Prepare GL state
		if (m_renderTexture) 
		{
			glViewport(0, 0, m_renderTexture->GetWidth(), m_renderTexture->GetHeight());
			m_renderTexture->Bind();
		}
		else 
		{
			glViewport(0, 0, m_width, m_height);
		}
		if (m_blend) 
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		if (m_backfaceCull) 
		{

			glEnable(GL_CULL_FACE);
		}
		if (m_depthTest) 
		{
			glEnable(GL_DEPTH_TEST);
		}

		// Draw model if we have one!
		if (m_model) 
		{
			m_model->Draw(m_modelMat, m_viewMat, m_projectionMat, m_camPos);
		}

		// Reset GL state
		if (m_renderTexture)
		{
			m_renderTexture->Unbind();
		}
		if (m_blend)
		{
			glDisable(GL_BLEND);
		}
		if (m_backfaceCull)
		{

			glDisable(GL_CULL_FACE);
		}
		if (m_depthTest)
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	void Renderer::Clear() 
	{
		// Specify the colour to clear the framebuffer to
		glClearColor(m_clearColour.x, m_clearColour.y, m_clearColour.z, m_clearColour.a);
		// This writes the above colour to the colour part of the framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}