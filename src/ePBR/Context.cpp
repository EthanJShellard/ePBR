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

const int DEFAULT_CUBEMAP_WIDTH = 2048;
const int DEFAULT_CONVOLUTED_CUBEMAP_WIDTH = 64;
const int DEFAULT_PREFILTER_CUBEMAP_WIDTH = 256;
const int DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH = 512;

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

		// Defaults
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
		m_window = SDL_CreateWindow("ePBR",  // The first parameter is the window title
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

		return std::shared_ptr<CubeMap>(new CubeMap(DEFAULT_CUBEMAP_WIDTH, _equirectangularMap, m_cubeMapGenerationShader));
	}

	// Inspired by https://learnopengl.com/PBR/IBL/Diffuse-irradiance
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, _environmentMap->m_mapID);

		// Matrices
		glUniformMatrix4fv(m_skyboxProjectionPos, 1, false, glm::value_ptr(_projectionMat));
		glUniformMatrix4fv(m_skyboxViewPos, 1, false, glm::value_ptr(_viewMat));

		// Ensure that we draw behind everything else!
		glDisable(GL_DEPTH_TEST);

		m_unitCube->Draw();
	}

	// Inspired by https://learnopengl.com/PBR/IBL/Diffuse-irradiance
	std::shared_ptr<CubeMap> Context::GenerateDiffuseIrradianceMap(std::shared_ptr<CubeMap> _cubeMap) 
	{
		if (!m_unitCube) 
		{
			m_unitCube = std::make_shared<Mesh>();
			m_unitCube->SetAsCube(0.5f);
		}
		if (!m_convolutionShader) 
		{
			m_convolutionShader = std::make_shared<Shader>(m_pwd + "data/shaders/environment_mapping/ConvoluteCubemap.vert", m_pwd + "data/shaders/environment_mapping/ConvoluteCubemap.frag");
			m_convolutionViewPos = glGetUniformLocation(m_convolutionShader->GetID(), "view");
			m_convolutionProjectionPos = glGetUniformLocation(m_convolutionShader->GetID(), "projection");
			m_convolutionEnvironementMapPos = glGetUniformLocation(m_convolutionShader->GetID(), "environmentMap");
		}

		std::shared_ptr<CubeMap> conv(new CubeMap());

		glGenFramebuffers(1, &conv->m_frameBufferID);
		glGenRenderbuffers(1, &conv->m_renderBufferID);

		glBindFramebuffer(GL_FRAMEBUFFER, conv->m_frameBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, conv->m_renderBufferID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, DEFAULT_CONVOLUTED_CUBEMAP_WIDTH, DEFAULT_CONVOLUTED_CUBEMAP_WIDTH);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, conv->m_renderBufferID);

		// Generate textures
		glGenTextures(1, &conv->m_mapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, conv->m_mapID);

		for (unsigned int i = 0; i < 6; i++)
		{
			// Presuming HDR for now
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, DEFAULT_CONVOLUTED_CUBEMAP_WIDTH, DEFAULT_CONVOLUTED_CUBEMAP_WIDTH, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		// View matrices targetting each side of a cube.
		glm::mat4 viewMatrices[] =
		{
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f))
		};

		glUseProgram(m_convolutionShader->GetID());
		glUniform1i(m_convolutionEnvironementMapPos, 0);
		glUniformMatrix4fv(m_convolutionProjectionPos, 1, false, glm::value_ptr(projectionMat));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMap->m_mapID);

		glViewport(0,0, DEFAULT_CONVOLUTED_CUBEMAP_WIDTH, DEFAULT_CONVOLUTED_CUBEMAP_WIDTH);
		glBindFramebuffer(GL_FRAMEBUFFER, conv->m_frameBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, conv->m_renderBufferID);

		glFrontFace(GL_CW);

		for (int i = 0; i < 6; i++) 
		{
			glUniformMatrix4fv(m_convolutionViewPos, 1, false, glm::value_ptr(viewMatrices[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, conv->m_mapID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_unitCube->Draw();
		}

		glFrontFace(GL_CCW);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		return conv;
	}
	 // Inspired by https://learnopengl.com/PBR/IBL/Specular-IBL
	std::shared_ptr<CubeMap> Context::GeneratePrefilterIrradianceMap(std::shared_ptr<CubeMap> _cubeMap)
	{
		if (!m_unitCube) 
		{
			m_unitCube = std::make_shared<Mesh>();
			m_unitCube->SetAsCube(0.5f);
		}
		if (!m_prefilteringShader) 
		{
			m_prefilteringShader = std::make_shared<Shader>(m_pwd + "data/shaders/environment_mapping/SpecularPrefilter.vert", m_pwd + "data/shaders/environment_mapping/SpecularPrefilter.frag");
			m_prefilterEnvironmentMapPos = glGetUniformLocation(m_prefilteringShader->GetID(), "environmentMap");
			m_prefilterProjectionPos = glGetUniformLocation(m_prefilteringShader->GetID(), "projection");
			m_prefilterViewPos = glGetUniformLocation(m_prefilteringShader->GetID(), "view");
			m_prefilterRoughnessPos = glGetUniformLocation(m_prefilteringShader->GetID(), "roughness");
		}

		std::shared_ptr<CubeMap> prefilterMap(new CubeMap());

		glGenFramebuffers(1, &prefilterMap->m_frameBufferID);
		glGenRenderbuffers(1, &prefilterMap->m_renderBufferID);

		glBindFramebuffer(GL_FRAMEBUFFER, prefilterMap->m_frameBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, prefilterMap->m_renderBufferID);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, prefilterMap->m_renderBufferID);

		glGenTextures(1, &prefilterMap->m_mapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap->m_mapID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, DEFAULT_PREFILTER_CUBEMAP_WIDTH, DEFAULT_PREFILTER_CUBEMAP_WIDTH, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Enable trilinear filtering
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		// View matrices targetting each side of a cube.
		glm::mat4 viewMatrices[] =
		{
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
			glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f))
		};

		glUseProgram(m_prefilteringShader->GetID());
		glUniformMatrix4fv(m_prefilterProjectionPos, 1, false, glm::value_ptr(projectionMat));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_prefilterEnvironmentMapPos, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMap->m_mapID);

		glBindFramebuffer(GL_FRAMEBUFFER, prefilterMap->m_frameBufferID);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; mip++) 
		{
			// Resize framebuffer according to mip-level size. Going to leverage gl texture filtering.
			unsigned int mipWidth = DEFAULT_PREFILTER_CUBEMAP_WIDTH * std::pow(0.5, mip);
			unsigned int mipHeight = DEFAULT_PREFILTER_CUBEMAP_WIDTH * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, prefilterMap->m_renderBufferID);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			glUniform1f(m_prefilterRoughnessPos, roughness);
			for (unsigned int i = 0; i < 6; i++) 
			{
				glUniformMatrix4fv(m_prefilterViewPos, 1, false, glm::value_ptr(viewMatrices[i]));
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap->m_mapID, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_unitCube->Draw();
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return prefilterMap;
	}

	// Inspired by https://learnopengl.com/PBR/IBL/Specular-IBL
	std::shared_ptr<Texture> Context::GetBRDFLookupTexture() 
	{
		if (m_BRDFLUT) return m_BRDFLUT;

		m_BRDFLUT = std::make_shared<Texture>();
		glGenTextures(1, &m_BRDFLUT->m_ID);

		GLuint fbo, rbo;

		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH, DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindTexture(GL_TEXTURE_2D, m_BRDFLUT->m_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH, DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLUT->m_ID, 0);

		glViewport(0,0, DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH, DEFAULT_BRDF_LOOK_UP_TEXTURE_WIDTH);
		
		std::shared_ptr<Shader> integrationMapShader = std::make_shared<Shader>(m_pwd + "data/shaders/environment_mapping/IntegrationMap.vert", m_pwd + "data/shaders/environment_mapping/IntegrationMap.frag");
		
		Mesh quad;
		quad.SetAsQuad(1.0f, 1.0f);
		
		glUseProgram(integrationMapShader->GetID());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		quad.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return m_BRDFLUT;
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
		m_skyboxViewPos(0),
		m_convolutionViewPos(0),
		m_convolutionProjectionPos(0),
		m_convolutionEnvironementMapPos(0),
		m_prefilterEnvironmentMapPos(0),
		m_prefilterProjectionPos(0),
		m_prefilterRoughnessPos(0),
		m_prefilterViewPos(0)
	{
	}

	Context::~Context() 
	{
		// Shut down GUI system
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		// Clean up openGL and SDL.
		//Consider leaving the SDL window around if it was provided in constructor.
		SDL_GL_DeleteContext(m_SDL_GL_Context);
		SDL_DestroyRenderer(m_SDL_Renderer);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}
}