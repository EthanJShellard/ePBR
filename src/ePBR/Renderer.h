#ifndef EPBR_RENDERER
#define EPBR_RENDERER

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

		// Flags
		bool m_depthTest;
		bool m_backfaceCull;
		bool m_blend;

		int m_width;
		int m_height;

	public:
		/// @brief Create a renderer with specified width and height which will render to whatever framebuffer is bound unless a RenderTexture is set.
		/// @param _width The width.
		/// @param _height The height.
		Renderer(int _width, int _height);

		/// @brief Create a renderer which will draw to the parameter render target. It will derive its width and height from that render target.
		/// @param _renderTarget A RenderTexture which will act as this Renderer's render target.
		Renderer(std::shared_ptr<RenderTexture> _renderTarget);

		/// @brief Draw using whatever parameters are set on this renderer.
		virtual void Draw();

		// Getters and setters past this point:
		
		/// @brief Set the dimensions of this renderer.
		/// @param _newWidth The new width.
		/// @param _newHeight The new height.
		void SetDimensions(int _newWidth, int _newHeight) { m_width = _newWidth; m_height = _newHeight; }

		/// @brief Get the dimensions of this renderer.
		/// @return A 2d vector of this renderer's dimensions.
		glm::vec2 GetDimensions() const { glm::vec2(m_width, m_height); }

		/// @brief Set the width of this renderer.
		/// @param _newWidth The new width.
		void SetWidth(int _newWidth) { m_width = _newWidth; }

		/// @brief Get the width of this renderer.
		/// @return The width of this renderer.
		int GetWidth() const { return m_width; }

		/// @brief Set the height of this renderer.
		/// @param _newHeight The new height.
		void SetHeight(int _newHeight) { m_height = _newHeight; }

		/// @brief Get the height of thie renderer.
		/// @return The height of this renderer.
		int GetHeight() const { return m_height; }

		/// @brief Set the target RenderTexture of this renderer.
		/// @param _newRenderTexture The new RenderTexture.
		void SetRenderTexture(std::shared_ptr<RenderTexture> _newRenderTexture) { m_renderTexture = _newRenderTexture; }

		/// @brief Get the target RenderTexture of this renderer. May be nullptr.
		/// @return The target RenderTexture of this renderer.
		std::shared_ptr<RenderTexture> GetRenderTexture() const { return m_renderTexture; }

		/// @brief Set the model this renderer will draw.
		/// @param _newModel The new model.
		void SetModel(std::shared_ptr<Model> _newModel) { m_model = _newModel; }

		/// @brief Get this renderer's current target model.
		/// @return The model.
		std::shared_ptr<Model> GetModel() const { return m_model; }

		/// @brief Set the projection matrix this renderer will use to draw.
		/// @param _newProjectionMat The new projection matrix.
		void SetProjectionMat(const glm::mat4& _newProjectionMat) { m_projectionMat = _newProjectionMat; }

		/// @brief Get this renderer's current projection matrix.
		/// @return The projection matrix.
		glm::mat4 GetProjectionMat() const { return m_projectionMat; }

		/// @brief Set the view matrix this renderer will use to draw.
		/// @param _newViewMat The new view matrix
		void SetViewMat(const glm::mat4& _newViewMat) { m_viewMat = _newViewMat; }

		/// @brief Get this renderer's current view matrix.
		/// @return The view matrix.
		glm::mat4 GetViewMat() const { return m_viewMat; }

		/// @brief Set the model matrix this renderer will use to draw.
		/// @param _newModelMat The new model matrix.
		void SetModelMat(const glm::mat4& _newModelMat) { m_modelMat = _newModelMat; }

		/// @brief Get this renderer's current model matrix.
		/// @return The model matrix.
		glm::mat4 GetModelMat() const { return m_modelMat; }

		/// @brief Set this renderer's core matrices.
		/// @param _modelMat The new model matrix.
		/// @param _viewMat The new view matrix.
		/// @param _projMat The new projection matrix.
		void SetMatrices(const glm::mat4& _modelMat, const glm::mat4& _viewMat, const glm::mat4& _projMat)
		{ m_projectionMat = _projMat; m_viewMat = _viewMat; m_modelMat = _modelMat; }

		/// @brief Set this renderer's camera position.
		/// @param _newCamPos The new camera position.
		void SetCamPos(const glm::vec3& _newCamPos) { m_camPos = _newCamPos; }

		/// @brief Get this renderer's current camera position.
		/// @return The camera position.
		glm::vec3 GetCamPos() const { return m_camPos; }

		/// @brief Set this renderer's OpenGL clear colour.
		/// @param _newClearColour The new clear colour.
		void SetClearColour(const glm::vec4& _newClearColour) { m_clearColour = _newClearColour; }

		/// @brief Get this renderer's current OpenGL clear colour.
		/// @return The clear colour.
		glm::vec4 GetClearColour() const { return m_clearColour; }

		/// @brief Clear the render target using the current clear colour.
		void Clear();

		/// @brief Set whether this renderer should perform depth tests when drawing. Disabling will cause new objects to always be drawn overtop others.
		/// @param _doDepthTest The new flag state.
		void SetFlagDepthTest(bool _doDepthTest) { m_depthTest = _doDepthTest; }

		/// @brief Get whether this renderer will enable depth testing when drawing.
		/// @return The flag state.
		bool GetFlagDepthTest() const { return m_depthTest; }

		/// @brief Set whether this renderer should enable backface culling.
		/// @param _doBackfaceCulling The new flag state.
		void SetFlagCullBackfaces(bool _doBackfaceCulling) { m_backfaceCull = _doBackfaceCulling; }

		/// @brief Get whether this renderer will enable backface culling when drawing.
		/// @return The flag state.
		bool GetFlagCullBackfaces() const { return m_backfaceCull; }

		/// @brief Set whether this renderer should peforming blending.
		/// @param _doBlending The new flag state.
		void SetFlagBlend(bool _doBlending) { m_blend = _doBlending; }

		/// @brief Get whether or not this renderer will perform blending.
		/// @return The flag state.
		bool GetFlagBlend() const { return m_blend; }
	};
}

#endif // EPBR_RENDERER