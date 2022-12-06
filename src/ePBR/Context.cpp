#include "Context.h"

// Glew linked statically
#include <GL/glew.h>
#include <glm/ext.hpp>

#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include "PBRMaterial.h"
#include "Model.h"
#include "CubeMap.h"
#include "Mesh.h"
#include "Shader.h"

#include <iostream>
#include <stdexcept>

namespace ePBR 
{
	void Context::InitGL() 
	{
		// GLEW has a problem with loading core OpenGL
		// See here: https://www.opengl.org/wiki/OpenGL_Loading_Library
		// The temporary workaround is to enable its 'experimental' features
		glewExperimental = GL_TRUE;

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			throw std::runtime_error((char *)glewGetErrorString(err));
		}
		std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

		std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	}

	void Context::InitSDL() 
	{
		// SDL_Init is the main initialisation function for SDL
		// It takes a 'flag' parameter which we use to tell SDL what systems we're going to use
		// Here, we want to use SDL's video system, so we give it the flag for this
		// Incidentally, this also initialises the input event system
		// This function also returns an error value if something goes wrong
		// So we can put this straight in an 'if' statement to check and exit if need be
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			// Something went very wrong in initialisation, all we can do is exit
			throw std::runtime_error("Whoops! Something went very wrong, cannot initialise SDL :(");
		}

		// Now we have got SDL initialised, we are ready to create a window!
		// These are some variables to help show you what the parameters are for this function
		// You can experiment with the numbers to see what they do
		int winPosX = 10;
		int winPosY = 10;
		m_window = SDL_CreateWindow("My Window!!!",  // The first parameter is the window title
			winPosX, winPosY,
			m_windowWidth, m_windowHeight,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		// The last parameter lets us specify a number of options
		// Here, we tell SDL that we want the window to be shown and that it can be resized
		// You can learn more about SDL_CreateWindow here: https://wiki.libsdl.org/SDL_CreateWindow?highlight=%28\bCategoryVideo\b%29|%28CategoryEnum%29|%28CategoryStruct%29
		// The flags you can pass in for the last parameter are listed here: https://wiki.libsdl.org/SDL_WindowFlags

		// The SDL_CreateWindow function returns an SDL_Window
		// This is a structure which contains all the data about our window (size, position, etc)
		// We will also need this when we want to draw things to the window
		// This is therefore quite important we don't lose it!
	}

	void Context::InitSDL_GL() 
	{
		// This is how we set the context profile
		// We need to do this through SDL, so that it can set up the OpenGL drawing context that matches this
		// (of course in reality we have no guarantee this will be available and should provide fallback methods if it's not!)
		// Anyway, we basically first say which version of OpenGL we want to use
		// So let's say 4.3:
		// Major version number (4):
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		// Minor version number (3):
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		// Then we say whether we want the core profile or the compatibility profile
		// Flag options are either: SDL_GL_CONTEXT_PROFILE_CORE   or   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
		// We'll go for the core profile
		// This means we are using the latest version and cannot use the deprecated functions
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// The SDL_Renderer is a structure that handles rendering
		// It will store all of SDL's internal rendering related settings
		// When we create it we tell it which SDL_Window we want it to render to
		// That renderer can only be used for this window
		// (yes, we can have multiple windows - feel free to have a play sometime)
		m_SDL_Renderer = SDL_CreateRenderer(m_window, -1, 0);

		// Now that the SDL renderer is created for the window, we can create an OpenGL context for it!
		// This will allow us to actually use OpenGL to draw to the window
		m_SDL_GL_Context = SDL_GL_CreateContext(m_window);
	}

	void Context::InitImGui() 
	{
		// Setting up the GUI system
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Set a dark theme
		ImGui::StyleColorsDark();

		// The GUI system is built with an OpenGL back-end, so need to connect it up
		// Also tell it which shader language version to target
		// #version 130 corresponds to OpenGL 3.0, so should be fine for us
		const char* glslVersion = "#version 130";
		ImGui_ImplSDL2_InitForOpenGL(m_window, m_SDL_GL_Context);
		ImGui_ImplOpenGL3_Init(glslVersion);
	}

	void Context::Init(SDL_Window* _window) 
	{
		if (_window) 
		{
			m_window = _window;
			SDL_GetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
		}
		else 
		{
			InitSDL();
		}

		InitSDL_GL();
		InitGL();
		InitImGui();

		m_initialised = true;
	}

	void Context::MaximiseWindow() 
	{
		SDL_MaximizeWindow(m_window);
		SDL_GetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
	}

	void Context::DisplayFrame() 
	{
		SDL_GL_SwapWindow(m_window);
	}

	std::shared_ptr<CubeMap> Context::GenerateCubemap(std::shared_ptr<Texture> _equirectangularMap) 
	{
		if (!m_cubeMapGenerationShader) 
		{
			m_cubeMapGenerationShader = std::make_shared<Shader>(
				m_pwd + "data\\shaders\\environment_mapping\\EquirectangularToCubemap.vert",
				m_pwd + "data\\shaders\\environment_mapping\\EquirectangularToCubemap.frag"
				);
		}

		return std::shared_ptr<CubeMap>(new CubeMap(_equirectangularMap, m_cubeMapGenerationShader));
	}

	void Context::RenderSkyBox(std::shared_ptr<CubeMap> _environmentMap, const glm::mat4& _viewMat, const glm::mat4& _projectionMat)
	{
		if (!m_unitCube) 
		{
			m_unitCube = std::make_shared<Mesh>();
			m_unitCube->SetAsCube(0.5f);
		}
		if (!m_skyboxShader) 
		{
			m_skyboxShader = std::make_shared<Shader>(m_pwd + "data/shaders/environment_mapping/Skybox.vert", m_pwd + "data/shaders/environment_mapping/Skybox.frag");
			m_skyboxProjectionPos = glGetUniformLocation(m_skyboxShader->GetID(), "projection");
			m_skyboxViewPos = glGetUniformLocation(m_skyboxShader->GetID(), "view");
			m_skyboxEnvironmentMapLocation = glGetUniformLocation(m_skyboxShader->GetID(), "environmentMap");
		}
	
		glUseProgram(m_skyboxShader->GetID());

		// Env map
		glUniform1i(m_skyboxEnvironmentMapLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _environmentMap->GetID());

		// Matrices
		glUniformMatrix4fv(m_skyboxProjectionPos, 1, false, glm::value_ptr(_projectionMat));
		glUniformMatrix4fv(m_skyboxViewPos, 1, false, glm::value_ptr(_viewMat));

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);

		m_unitCube->Draw();
	}

	Context::Context(std::string _projectWorkingDirectory) :
		m_SDL_Renderer(NULL),
		m_window(NULL),
		m_SDL_GL_Context(),
		m_initialised(false),
		m_windowWidth(1920),
		m_windowHeight(1080),
		m_pwd(_projectWorkingDirectory),
		m_skyboxEnvironmentMapLocation(0),
		m_skyboxProjectionPos(0),
		m_skyboxViewPos(0)
	{
	}

	Context::~Context() 
	{
		// Shut down GUI system
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		// Our cleanup phase, hopefully fairly self-explanatory ;)
		SDL_GL_DeleteContext(m_SDL_GL_Context);
		SDL_DestroyRenderer(m_SDL_Renderer);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}
}