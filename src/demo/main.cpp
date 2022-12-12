#include <ePBR/ePBR.h>

#include <glm/ext.hpp>

#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <stdexcept>
#include <iostream>

int main(int argc, char* argv[])
{
	try 
	{
		std::string pwd(argv[0]);
		pwd = pwd.substr(0, pwd.find_last_of('\\') + 1);

		ePBR::Context context(pwd);
		context.Init(nullptr);
		context.MaximiseWindow();

		// Set up matrices
		glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.5f));
		glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)context.GetWindowWidth() / context.GetWindowHeight(), 0.1f, 100.0f);
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));

		// Set up renderer
		ePBR::Renderer renderer(context.GetWindowWidth(), context.GetWindowHeight());
		renderer.SetFlagCullBackfaces(false);
		renderer.SetFlagDepthTest(true);
		glm::vec3 camPos(0);

		// Test PBR Material
		std::shared_ptr<ePBR::PBRMaterial> material = std::make_shared<ePBR::PBRMaterial>();
		material->SetAlbedoTexture(pwd + "data\\textures\\rustediron2\\rustediron2_basecolor.png");

		material->SetMetalnessMap(pwd + "data\\textures\\rustediron2\\rustediron2_metallic.png");
		material->SetNormalMap(pwd + "data\\textures\\rustediron2\\rustediron2_normal.png");
		material->SetRoughnessMap(pwd + "data\\textures\\rustediron2\\rustediron2_roughness.png");
		material->LoadShaders(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBRIBL.frag");

		
		std::shared_ptr<ePBR::CubeMap> cubeMap1, convolutedCubeMap1, prefilterEnvMap1;
		{
			std::shared_ptr<ePBR::Texture> equirectangularMap1 = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\EnvironmentMaps\\HDR_029_Sky_Cloudy_Ref.hdr", true);
			cubeMap1 = context.GenerateCubemap(equirectangularMap1);
			convolutedCubeMap1 = context.GenerateDiffuseIrradianceMap(cubeMap1);
			prefilterEnvMap1 = context.GeneratePrefilterIrradianceMap(cubeMap1);
		}

		// Get second equirectangular map and generate cubemap
		std::shared_ptr<ePBR::CubeMap> cubeMap2, convolutedCubeMap2, prefilterEnvMap2;
		{
			std::shared_ptr<ePBR::Texture> equirectangularMap2 = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\EnvironmentMaps\\Old town by nite.jpg", true);
			cubeMap2 = context.GenerateCubemap(equirectangularMap2);
			convolutedCubeMap2 = context.GenerateDiffuseIrradianceMap(cubeMap2);
			prefilterEnvMap2 = context.GeneratePrefilterIrradianceMap(cubeMap2);
		}

		std::shared_ptr<ePBR::CubeMap> selectedSkybox = cubeMap1;

		std::shared_ptr<ePBR::Texture> brdfLUT = context.GetBRDFLookupTexture();

		material->SetEnvironmentMap(convolutedCubeMap1);
		material->SetPrefilterEnvironmentMap(prefilterEnvMap1);
		material->SetBRDFLookupTexture(brdfLUT);

		// The mesh is the geometry for the object
		std::shared_ptr<ePBR::Mesh> modelMesh = std::make_shared<ePBR::Mesh>();
		//// Load from OBJ file. This must have triangulated geometry
		modelMesh->LoadOBJ(pwd + "data\\models\\sphere\\triangulated.obj");
		//modelMesh->SetAsCube(0.5f);
		//material->SetAlbedoTexture(brdfLUT);

		// Set up model
		std::shared_ptr<ePBR::Model> testModel = std::make_shared<ePBR::Model>();
		testModel->SetMesh(0, modelMesh);
		testModel->SetMaterial(0, material);

		// Controls
		bool cmdRotateDown(false), cmdRotateUp(false), cmdRotateLeft(false), cmdRotateRight(false);
		float cameraAngleX(0), cameraAngleY(0);
		bool useIBLShader = true;
		bool showIMGUI = true;
		bool currentScene = true;

		// Timing
		unsigned int lastTime = SDL_GetTicks();

		bool running = true;
		while (running) 
		{
			SDL_Event incomingEvent;
			while (SDL_PollEvent(&incomingEvent))
			{
				// Let our GUI system handle some events
				ImGui_ImplSDL2_ProcessEvent(&incomingEvent);

				switch (incomingEvent.type)
				{
				case SDL_QUIT:
					running = false;
					break;

				case SDL_KEYDOWN:
					// The event type is SDL_KEYDOWN
					// This means that the user has pressed a key
					// Let's figure out which key they pressed
					switch (incomingEvent.key.keysym.sym)
					{
					case SDLK_DOWN:
						cmdRotateDown = true;
						break;
					case SDLK_UP:
						cmdRotateUp = true;
						break;
					case SDLK_LEFT:
						cmdRotateLeft = true;
						break;
					case SDLK_RIGHT:
						cmdRotateRight = true;
						break;
					}
					break;

				case SDL_KEYUP:
					// The event type is SDL_KEYUP
					// This means that the user has released a key
					// Let's figure out which key they released
					switch (incomingEvent.key.keysym.sym)
					{
					case SDLK_DOWN:
						cmdRotateDown = false;
						break;
					case SDLK_UP:
						cmdRotateUp = false;
						break;
					case SDLK_LEFT:
						cmdRotateLeft = false;
						break;
					case SDLK_RIGHT:
						cmdRotateRight = false;
						break;
					}
					break;
				}
			}

			// Timing
			unsigned int currentTime = SDL_GetTicks();
			float deltaTime = (float)(currentTime - lastTime) / 1000.0f;
			lastTime = currentTime;

			cameraAngleY += cmdRotateLeft ? 1.0f * deltaTime : 0.0f;
			cameraAngleY += cmdRotateRight ? -1.0f * deltaTime : 0.0f;

			cameraAngleX += cmdRotateDown ? -1.0f * deltaTime : 0.0f;
			cameraAngleX += cmdRotateUp ? 1.0f * deltaTime : 0.0f;

			modelMatrix = glm::rotate(modelMatrix, deltaTime * 1, glm::vec3(0, 1.0f, 0));

			// Construct view matrix
			viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -2.0f)), cameraAngleX, glm::vec3(1, 0, 0)), cameraAngleY, glm::vec3(0, 1, 0));
			camPos = glm::inverse(viewMatrix) * glm::mat4(1) * glm::vec4(0, 0, 0, 1);

			renderer.Clear();

			renderer.SetMatrices(modelMatrix, viewMatrix, projectionMatrix);
			renderer.SetCamPos(camPos);
			renderer.SetModel(testModel);
			renderer.Draw();

			context.RenderSkyBox(selectedSkybox, viewMatrix, projectionMatrix);

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			if (showIMGUI)
			{
				// Create a window, give it a name
				// All ImGui commands after this to create widgets will be added to the window
				ImGui::Begin("Controls", NULL, ImGuiWindowFlags_AlwaysAutoResize);

				// This is how you add a bit of text to the window
				ImGui::Text("Arrow keys rotate the camera");

				// Scene switching
				if (ImGui::Button("Switch scene")) 
				{
					if (currentScene) 
					{
						material->SetEnvironmentMap(convolutedCubeMap2);
						material->SetPrefilterEnvironmentMap(prefilterEnvMap2);
						selectedSkybox = cubeMap2;
						currentScene = !currentScene;
					}
					else 
					{
						material->SetEnvironmentMap(convolutedCubeMap1);
						material->SetPrefilterEnvironmentMap(prefilterEnvMap1);
						selectedSkybox = cubeMap1;
						currentScene = !currentScene;
					}
				}

				// Shader switching
				if (ImGui::Button( useIBLShader ? "Reload IBL only shader (current)" : "Use IBL only shader")) 
				{
					useIBLShader = true;
					material->LoadShaders(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBRIBL.frag");
				}
				if (ImGui::Button(!useIBLShader ? "Reload direct lighting only shader (current)" : "Use direct lighting only shader")) 
				{
					useIBLShader = false;
					material->LoadShaders(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBRDirectLighting.frag");
				}

				// Display FPS
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				// We've finished adding stuff to the window
				ImGui::End();
			}

			// Render GUI to screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			context.DisplayFrame();	
		}
	}
	catch (std::runtime_error e) 
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}