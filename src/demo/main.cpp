#include <ePBR/ePBR.h>

#include <glm/ext.hpp>

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
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,0.5,0));

		// Set up renderer
		ePBR::Renderer renderer(context.GetWindowWidth(), context.GetWindowHeight());
		renderer.SetFlagCullBackfaces(true);
		renderer.SetFlagDepthTest(true);
		glm::vec3 camPos(0);

		// Test PBR Material
		std::shared_ptr<ePBR::PBRMaterial> material = std::make_shared<ePBR::PBRMaterial>();
		material->SetAlbedoTexture(pwd + "data\\textures\\rustediron2\\rustediron2_basecolor.png");
		// CUBE TEST
		//material->SetAlbedoTexture(pwd + "data\\textures\\EnvironmentMaps\\Old town by nite.jpg", true);

		material->SetMetalnessMap(pwd + "data\\textures\\rustediron2\\rustediron2_metallic.png");
		material->SetNormalMap(pwd + "data\\textures\\rustediron2\\rustediron2_normal.png");
		material->SetRoughnessMap(pwd + "data\\textures\\rustediron2\\rustediron2_roughness.png");
		material->LoadShaders(pwd + "data\\shaders\\PBRVert.txt", pwd + "data\\shaders\\PBRFrag.txt");

		// The mesh is the geometry for the object
		std::shared_ptr<ePBR::Mesh> modelMesh = std::make_shared<ePBR::Mesh>();
		modelMesh->SetAsCube(0.5f);
		//// Load from OBJ file. This must have triangulated geometry
		//modelMesh->LoadOBJ(pwd + "data\\models\\sphere\\triangulated.obj");

		// Set up model
		std::shared_ptr<ePBR::Model> testModel = std::make_shared<ePBR::Model>();
		testModel->SetMesh(0, modelMesh);
		testModel->SetMaterial(0, material);

		std::shared_ptr<ePBR::Shader> shader = std::make_shared<ePBR::Shader>(pwd + "data\\shaders\\PBRVert.txt", pwd + "data\\shaders\\PBRFrag.txt");
		//std::shared_ptr<ePBR::Shader> shader = std::make_shared<ePBR::Shader>(pwd + "data\\shaders\\environment_mapping\\EquirectangularToCubemap.vert", pwd + "data\\shaders\\environment_mapping\\EquirectangularToCubemap.frag");
		GLuint projLocation = glGetUniformLocation(shader->GetID(), "projection");
		GLuint viewLocation = glGetUniformLocation(shader->GetID(), "view");

		// Get equirectangular map
		std::shared_ptr<ePBR::Texture> equirectangularMap = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\EnvironmentMaps\\Old town by nite.jpg", true);
		std::shared_ptr<ePBR::CubeMap> cubeMap = context.GenerateCubemap(equirectangularMap);

		for (auto model : testModel->GetMaterials())
		{
			model->SetShader(shader);
		}

		// Controls
		bool cmdRotateDown(false), cmdRotateUp(false), cmdRotateLeft(false), cmdRotateRight(false);
		float cameraAngleX(0), cameraAngleY(0);

		// Timing
		unsigned int lastTime = SDL_GetTicks();

		// Render cubemap faces into framebuffers textures


		bool running = true;
		while (running) 
		{
			SDL_Event incomingEvent;
			while (SDL_PollEvent(&incomingEvent))
			{
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

			modelMatrix = glm::rotate(modelMatrix, deltaTime * 1, glm::vec3(0.5, 0, 0.5));

			// Construct view matrix
			viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -2.0f)), cameraAngleX, glm::vec3(1, 0, 0)), cameraAngleY, glm::vec3(0, 1, 0));
			camPos = glm::inverse(viewMatrix) * glm::mat4(1) * glm::vec4(0, 0, 0, 1);

			renderer.Clear();

			renderer.SetMatrices(modelMatrix, viewMatrix, projectionMatrix);
			renderer.SetCamPos(camPos);
			renderer.SetModel(testModel);
			renderer.Draw();

			context.RenderSkyBox(cubeMap, viewMatrix, projectionMatrix);

			context.DisplayFrame();	
		}
	}
	catch (std::runtime_error e) 
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}