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

		// Convolution
		std::shared_ptr<Shader> m_convolutionShader;
		unsigned int m_convolutionViewPos;
		unsigned int m_convolutionProjectionPos;
		unsigned int m_convolutionEnvironementMapPos;

		// Prefiltering
		std::shared_ptr<Shader> m_prefilteringShader;
		unsigned int m_prefilterEnvironmentMapPos;
		unsigned int m_prefilterProjectionPos;
		unsigned int m_prefilterRoughnessPos;
		unsigned int m_prefilterViewPos;

		// BRDF Lookup
		std::shared_ptr<Texture> m_BRDFLUT;

		int m_windowWidth;
		int m_windowHeight;

		bool m_initialised;

		/// @brief Initialise glew.
		void InitGL();
		/// @brief Initialise SDL.
		void InitSDL();
		/// @brief Initialise the SDL renderer and create the SDL_GL context.
		void InitSDL_GL();
		/// @brief Initialise ImGui.
		void InitImGui();
	public:
		/// @brief Initialise this ePBR context. Must be called before anything can be rendered.
		/// @param _window The window this context should render to. If a null pointer is provided as argument then a window will be created.
		void Init(SDL_Window* _window);

		/// @brief Maximise the window. Updates window width and height variables which can be used to update Renderers and RenderTextures.
		void MaximiseWindow();

		/// @brief Get the width of the target SDL window.
		/// @return The width of the target SDL window.
		int GetWindowWidth() const { return m_windowWidth; }

		/// @brief Get the height of the target SDL window.
		/// @return The height of the target SDL window.
		int GetWindowHeight() const { return m_windowHeight; }

		/// @brief Disply the rendered frame in the target SDL window. Should be called after all render calls.
		void DisplayFrame();

		/// @brief Generate a CubeMap from an equirecatangular environment map.
		/// @param _equirectangularMap The equirectangular map which should be used to generate the cubemap. Expected to be in HDR format.
		/// @return The newly generated CubeMap.
		std::shared_ptr<CubeMap> GenerateCubemap(std::shared_ptr<Texture> _equirectangularMap);

		/// @brief Draw a skybox behind all other rendered objects.
		/// @param _cubeMap The CubeMap which will be drawn as a skybox.
		/// @param _viewMat A matrix representing the position and orientation of the viewpoint from which the skybox will be drawn. Typically that of the main 'camera'.
		/// @param _projectionMat A matrix representing the projection that will be used to draw the skybox. Typically that of the main 'camera'.
		void RenderSkyBox(std::shared_ptr<CubeMap> _cubeMap, const glm::mat4& _viewMat, const glm::mat4& _projectionMat);
		
		/// @brief Generate a diffuse irradiance map from a cubemap via convolution.
		/// @param _cubeMap The cubemap to be convoluted.
		/// @return A newly-generated, convoluted, diffuse irradiance map.
		std::shared_ptr<CubeMap> GenerateDiffuseIrradianceMap(std::shared_ptr<CubeMap> _cubeMap);

		/// @brief Generate a 'specular' prefilter irradiance map from a cubemap.
		/// @param _cubeMap The environment map which will be processed to create the prefilter irradiance map.
		/// @return A newly-generated prefilter irradiance map.
		std::shared_ptr<CubeMap> GeneratePrefilterIrradianceMap(std::shared_ptr<CubeMap> _cubeMap);

		/// @brief Retrieve the BRDF lookup texture, used as part of specular image based lighting.
		/// @return The BRDF lookup texture.
		std::shared_ptr<Texture> GetBRDFLookupTexture();

		/// @brief Construct an ePBR context. Init() will need to be called before rendering can be done.
		/// @param _projectWorkingDirectory The project working directory - the location of the program's executable and data directory.
		Context(std::string _projectWorkingDirectory);
		~Context();
	};
}

#endif // EPBR_CONTEXT