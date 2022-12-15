#include <ePBR/ePBR.h>

#include <glm/ext.hpp>

#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <stdexcept>
#include <iostream>
#include <chrono>

struct Scene
{
	std::vector<std::shared_ptr<ePBR::Model>> models;
	std::vector<glm::vec3> modelPositions;
	float cameraDistance;
};

int main(int argc, char* argv[])
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
	SDL_GL_SetSwapInterval(0); // Disable vsync
	glm::vec3 camPos(0);

	// Load Shaders
	std::shared_ptr<ePBR::Shader> comboPBRShader, IBLOnlyShader, directLightingOnlyShader, noSamplersShader, blinnPhongShader;
	comboPBRShader = std::make_shared<ePBR::Shader>(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBR.frag");
	IBLOnlyShader = std::make_shared<ePBR::Shader>(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBRIBL.frag");
	directLightingOnlyShader = std::make_shared<ePBR::Shader>(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBRDirectLighting.frag");
	noSamplersShader = std::make_shared<ePBR::Shader>(pwd + "data\\shaders\\PBR.vert", pwd + "data\\shaders\\PBRNoSamplers.frag");
	blinnPhongShader = std::make_shared<ePBR::Shader>(pwd + "data/shaders/BlinnPhong.vert", pwd + "data/shaders/BlinnPhong.frag");

	// Get first equirectangular map and generate cubemap
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

	// Load textures
	auto albedoTex = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\rustediron2\\rustediron2_basecolor.png");
	auto metalnessTex = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\rustediron2\\rustediron2_metallic.png");
	auto normalMap = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\rustediron2\\rustediron2_normal.png");
	auto roughnessTex = std::make_shared<ePBR::Texture>(pwd + "data\\textures\\rustediron2\\rustediron2_roughness.png");

	// CREATE MATERIALS
	// IBL Material
	std::shared_ptr<ePBR::PBRMaterial> IBLMaterial = std::make_shared<ePBR::PBRMaterial>();
	IBLMaterial->SetAlbedoTexture(albedoTex);
	IBLMaterial->SetMetalnessMap(metalnessTex);
	IBLMaterial->SetNormalMap(normalMap);
	IBLMaterial->SetRoughnessMap(roughnessTex);
	IBLMaterial->SetShader(IBLOnlyShader);
	IBLMaterial->SetIrradianceMap(convolutedCubeMap1);
	IBLMaterial->SetPrefilterEnvironmentMap(prefilterEnvMap1);
	IBLMaterial->SetBRDFLookupTexture(brdfLUT);

	// PBR direct lighting material
	std::shared_ptr<ePBR::PBRMaterial> directLightingMaterial = std::make_shared<ePBR::PBRMaterial>();
	directLightingMaterial->SetAlbedoTexture(albedoTex);
	directLightingMaterial->SetMetalnessMap(metalnessTex);
	directLightingMaterial->SetNormalMap(normalMap);
	directLightingMaterial->SetRoughnessMap(roughnessTex);
	directLightingMaterial->SetShader(directLightingOnlyShader);

	// legacy material
	std::shared_ptr<ePBR::LegacyMaterial> legacyMaterial = std::make_shared<ePBR::LegacyMaterial>();
	legacyMaterial->SetAlbedoTexture(albedoTex);
	legacyMaterial->SetNormalMap(normalMap);
	legacyMaterial->SetShininess(50.0f);
	legacyMaterial->SetShader(blinnPhongShader);
	// MATERIAL CREATION DONE

	// Set up mesh
	std::shared_ptr<ePBR::Mesh> modelMesh = std::make_shared<ePBR::Mesh>();
	modelMesh->LoadOBJ(pwd + "data\\models\\sphere\\triangulated.obj");

	// Set up model
	std::shared_ptr<ePBR::Model> testModel = std::make_shared<ePBR::Model>();
	testModel->SetMesh(0, modelMesh);
	testModel->SetMaterial(0, IBLMaterial);

	// SET UP SCENES
	Scene arrayOfSpheresScene;
	arrayOfSpheresScene.cameraDistance = 7.0f;
	int sphereSpacing(1.85f);
	for (int x = 0; x < 5; x++) 
		{
			for (int y = 0; y < 5; y++) 
			{
				std::shared_ptr<ePBR::PBRMaterial> mat = std::make_shared<ePBR::PBRMaterial>();
				std::shared_ptr<ePBR::Model> model = std::make_shared<ePBR::Model>();
				* mat = * IBLMaterial;
				*model = *testModel;

				mat->SetAlbedo(glm::vec3(1.0f, 0.0f, 0.0f));
				mat->SetMetalness(0.95f - (x / 5.0f));
				mat->SetRoughness((y / 5.0f) + 0.05f);
				mat->SetShader(noSamplersShader);
				model->SetMaterial(0, mat);

				arrayOfSpheresScene.models.push_back(model);
				arrayOfSpheresScene.modelPositions.push_back(glm::vec3( ((-2 * sphereSpacing) + (sphereSpacing * x)), ((-2 * sphereSpacing) + (sphereSpacing * y)), 0.0f ));
			}
		}

	Scene singleSphereScene;
	singleSphereScene.models.assign(1, testModel);
	singleSphereScene.cameraDistance = (2.0f);
	singleSphereScene.modelPositions.push_back(glm::vec3(0.0f));

	Scene modelComparisonScene;
	std::shared_ptr<ePBR::Model> sph1(new ePBR::Model()), sph2(new ePBR::Model()), sph3(new ePBR::Model());
	*sph1 = *sph2 = *sph3 = *testModel;
	sph1->SetMaterial(0, IBLMaterial);
	sph2->SetMaterial(0, directLightingMaterial);
	sph3->SetMaterial(0, legacyMaterial);
	modelComparisonScene.models = {sph1, sph2, sph3};
	modelComparisonScene.modelPositions = { glm::vec3(-2, 0, 0) , glm::vec3(0, 0, 0), glm::vec3(2, 0, 0) };
	modelComparisonScene.cameraDistance = 4.0f;
	// SCENE SETUP COMPLETE

	// Controls
	bool cmdRotateDown(false), cmdRotateUp(false), cmdRotateLeft(false), cmdRotateRight(false);
	float cameraAngleX(0), cameraAngleY(0);
	bool showIMGUI = true;
	bool isDayEnvironment = true;

	Scene* currentScene = &modelComparisonScene;

	// Timing
	uint64_t lastTime = SDL_GetTicks();
	std::chrono::time_point<std::chrono::system_clock> beginTP = std::chrono::system_clock::now();
	uint64_t numTicks = 0;

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

		// Clear render target
		renderer.Clear();

		// Draw skybox before anything else
		context.RenderSkyBox(selectedSkybox, viewMatrix, projectionMatrix);

		// Timing
		uint64_t currentTime = SDL_GetTicks();
		float deltaTime = (float)(currentTime - lastTime) / 1000.0f;
		numTicks++;
		lastTime = currentTime;

		cameraAngleY += cmdRotateLeft ? 1.0f * deltaTime : 0.0f;
		cameraAngleY += cmdRotateRight ? -1.0f * deltaTime : 0.0f;

		cameraAngleX += cmdRotateDown ? -1.0f * deltaTime : 0.0f;
		cameraAngleX += cmdRotateUp ? 1.0f * deltaTime : 0.0f;

		// Construct view matrix
		viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -currentScene->cameraDistance)), cameraAngleX, glm::vec3(1, 0, 0)), cameraAngleY, glm::vec3(0, 1, 0));
		camPos = glm::inverse(viewMatrix) * glm::mat4(1) * glm::vec4(0, 0, 0, 1);
		renderer.SetCamPos(camPos);
		renderer.SetProjectionMat(projectionMatrix);
		renderer.SetViewMat(viewMatrix);

		// Draw all objects in scene
		for (int i = 0; i < currentScene->models.size(); i++) 
		{
			modelMatrix = glm::translate(glm::mat4(1), currentScene->modelPositions[i]);
			renderer.SetModelMat(modelMatrix);
			renderer.SetModel(currentScene->models[i]);
			renderer.Draw();
		}

		if (showIMGUI)
			{
				// Start the Dear ImGui frame
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();

				// Create a window, give it a name
				// All ImGui commands after this to create widgets will be added to the window
				ImGui::Begin("Controls", NULL, ImGuiWindowFlags_AlwaysAutoResize);

				// This is how you add a bit of text to the window
				ImGui::Text("Arrow keys rotate the camera");

				ImGui::Text("Select a scene:");
				if (currentScene != &modelComparisonScene && ImGui::Button("Switch to model comparison scene")) 
				{
					currentScene = &modelComparisonScene;

					// Make sure environment map is up to date
					for (auto model : currentScene->models)
					{
						std::shared_ptr<ePBR::PBRMaterial> mat = std::dynamic_pointer_cast<ePBR::PBRMaterial>(model->GetMaterials()[0]);
						if (mat)
						{
							mat->SetIrradianceMap(isDayEnvironment ? convolutedCubeMap1 : convolutedCubeMap2);
							mat->SetPrefilterEnvironmentMap(isDayEnvironment ? prefilterEnvMap1 : prefilterEnvMap2);
						}
					}
				}
				else if (currentScene != &singleSphereScene && ImGui::Button("Switch to single sphere scene"))
				{
					currentScene = &singleSphereScene;

					// Make sure environment map is up to date
					for (auto model : currentScene->models)
					{
						std::shared_ptr<ePBR::PBRMaterial> mat = std::dynamic_pointer_cast<ePBR::PBRMaterial>(model->GetMaterials()[0]);
						if (mat)
						{
							mat->SetIrradianceMap(isDayEnvironment ? convolutedCubeMap1 : convolutedCubeMap2);
							mat->SetPrefilterEnvironmentMap(isDayEnvironment ? prefilterEnvMap1 : prefilterEnvMap2);
						}
					}
				}
				else if (currentScene != &arrayOfSpheresScene && ImGui::Button("Switch to array of spheres scene"))
				{
					currentScene = &arrayOfSpheresScene;
				}

				// Environment map switching
				ImGui::Text("Manage environment map:");
				if (ImGui::Button("Swap environment map"))
				{
					if (isDayEnvironment)
					{
						for (auto model : currentScene->models) 
						{
							std::shared_ptr<ePBR::PBRMaterial> mat = std::dynamic_pointer_cast<ePBR::PBRMaterial>(model->GetMaterials()[0]);
							if (mat) 
							{
								mat->SetIrradianceMap(convolutedCubeMap2);
								mat->SetPrefilterEnvironmentMap(prefilterEnvMap2);
							}
						}
						selectedSkybox = cubeMap2;
						isDayEnvironment = !isDayEnvironment;
					}
					else
					{
						for (auto model : currentScene->models)
						{
							std::shared_ptr<ePBR::PBRMaterial> mat = std::dynamic_pointer_cast<ePBR::PBRMaterial>(model->GetMaterials()[0]);
							if (mat) 
							{
								mat->SetIrradianceMap(convolutedCubeMap1);
								mat->SetPrefilterEnvironmentMap(prefilterEnvMap1);
							}
						}
						selectedSkybox = cubeMap1;
						isDayEnvironment = !isDayEnvironment;
					}
				}

				// Display FPS
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				// We've finished adding stuff to the window
				ImGui::End();

				// Render GUI to screen
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}

		context.DisplayFrame();	
	}

	std::chrono::time_point<std::chrono::system_clock> endTP = std::chrono::system_clock::now();
	std::chrono::duration<float> fsec, fms;
	fsec = endTP - beginTP;
	fms = std::chrono::duration_cast<std::chrono::milliseconds>(fsec) * 1000;
	std::cout << "Average frame time  = " << fms.count() / numTicks << "ms.\n Average FPS = " << 1.0f / (fsec.count() / numTicks);

	return 0;
}