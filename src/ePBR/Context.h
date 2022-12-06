#ifndef EPBR_CONTEXT
#define EPBR_CONTEXT

#include <string>
#include <memory>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

// GUI system: https://github.com/ocornut/imgui
// - prevent compile error by building with: WINDOWS_IGNORE_PACKING_MISMATCH
#include <imgui/imgui.h>

namespace ePBR 
{
	class CubeMap;
	class Texture;
	class Mesh;
	class Shader;

	class Context 
	{
	private:
		SDL_Window* m_window;
		SDL_Renderer* m_SDL_Renderer;
		SDL_GLContext m_SDL_GL_Context;

		std::string m_pwd;

		// Cubemap generation
		std::shared_ptr<Shader> m_cubeMapGenerationShader;
		
		// Skybox
		std::shared_ptr<Mesh> m_unitCube;
		std::shared_ptr<Shader> m_skyboxShader;
		unsigned int m_skyboxViewPos;
		unsigned int m_skyboxProjectionPos;
		unsigned int m_skyboxEnvironmentMapLocation;

		int m_windowWidth;
		int m_windowHeight;

		bool m_initialised;

		void InitGL();
		void InitSDL();
		void InitSDL_GL();
		void InitImGui();
	public:
		void Init(SDL_Window* _window);

		/// @brief Maximise the window. Updates window width and height variables which can be used to update Renderers and RenderTextures
		void MaximiseWindow();

		int GetWindowWidth() const { return m_windowWidth; }
		int GetWindowHeight() const { return m_windowHeight; }

		void DisplayFrame();

		std::shared_ptr<CubeMap> GenerateCubemap(std::shared_ptr<Texture> _equirectangularMap);
		void RenderSkyBox(std::shared_ptr<CubeMap> _environmentMap, const glm::mat4& _viewMat, const glm::mat4& _projectionMat);

		Context(std::string _projectWorkingDirectory);
		~Context();
	};
}

#endif // EPBR_CONTEXT