#include <memory>
#include <glm/glm.hpp>

#include "Model.h"
#include "RenderTexture.h"

namespace ePBR 
{
	class Renderer 
	{
		std::shared_ptr<RenderTexture> m_renderTexture;
		std::shared_ptr<Model> m_model;

		glm::mat4 m_projectionMat;
		glm::mat4 m_viewMat;
		glm::mat4 m_modelMat;
		glm::vec3 m_camPos;

		glm::vec4 m_clearColour;

		bool m_depthTest;
		bool m_backfaceCull;
		bool m_blend;

		int m_width;
		int m_height;

	public:
		Renderer(int _width, int _height);
		Renderer(std::shared_ptr<RenderTexture> _renderTarget);

		virtual void Draw();

		// Getters and setters past this point:
		void SetDimensions(int _newWidth, int _newHeight) { m_width = _newWidth; m_height = _newHeight; }
		glm::vec2 GetDimensions() const { glm::vec2(m_width, m_height); }
		void SetWidth(int _newWidth) { m_width = _newWidth; }
		int GetWidth() const { return m_width; }
		void SetHeight(int _newHeight) { m_height = _newHeight; }
		int GetHeight() const { return m_height; }

		void SetRenderTexture(std::shared_ptr<RenderTexture> _newRenderTexture) { m_renderTexture = _newRenderTexture; }
		std::shared_ptr<RenderTexture> GetRenderTexture() const { return m_renderTexture; }

		void SetModel(std::shared_ptr<Model> _newModel) { m_model = _newModel; }
		std::shared_ptr<Model> GetModel() const { return m_model; }

		void SetProjectionMat(const glm::mat4& _newProjectionMat) { m_projectionMat = _newProjectionMat; }
		glm::mat4 GetProjectionMat() const { return m_projectionMat; }
		void SetViewMat(const glm::mat4& _newViewMat) { m_viewMat = _newViewMat; }
		glm::mat4 GetViewMat() const { return m_viewMat; }
		void SetModelMat(const glm::mat4& _newModelMat) { m_modelMat = _newModelMat; }
		glm::mat4 GetModelMat() const { return m_modelMat; }

		void SetMatrices(const glm::mat4& _modelMat, const glm::mat4& _viewMat, const glm::mat4& _projMat)
		{ m_projectionMat = _projMat; m_viewMat = _viewMat; m_modelMat = _modelMat; }

		void SetCamPos(const glm::vec3& _newCamPos) { m_camPos = _newCamPos; }
		glm::vec3 GetCamPos() const { return m_camPos; }

		void SetClearColour(const glm::vec4& _newClearColour) { m_clearColour = _newClearColour; }
		glm::vec4 GetClearColour() const { return m_clearColour; }
		void Clear();

		void SetFlagDepthTest(bool _doDepthTest) { m_depthTest = _doDepthTest; }
		bool GetFlagDepthTest() const { return m_depthTest; }

		void SetFlagCullBackfaces(bool _doBackfaceCulling) { m_backfaceCull = _doBackfaceCulling; }
		bool GetFlagCullBackfaces() const { return m_backfaceCull; }

		void SetFlagBlend(bool _doBlending) { m_blend = _doBlending; }
		bool GetFlagBlend() const { return m_blend; }
	};
}