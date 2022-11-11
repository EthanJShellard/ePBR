
#include "Scene.h"

#include <iostream>
#include <SDL2/SDL.h>

#include "PBRMaterial.h"

namespace ePBR 
{

	Scene::Scene(const std::string& _pwd)
	{
		m_cameraAngleX = 0.0f, m_cameraAngleY = 0.0f;

		// Set up the viewing matrix
		// This represents the camera's orientation and position
		m_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.5f));


		// Set up a projection matrix
		// Manually set aspect for now, just for testing
		m_projMatrix = glm::perspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);




		// Set up your scene here

		// Position of the light, in world-space
		m_lightPosition = glm::vec3(10, 10, 0);

		// Create a game object
		// This needs a material and a mesh
		m_model = std::make_shared<GameObject>();

		// Create the material for the game object
		//std::shared_ptr<Material> modelMaterial = std::make_shared<Material>();
		//// Shaders are now in files
		//modelMaterial->LoadShaders(_pwd + "data\\shaders\\vertShader.txt", _pwd + "data\\shaders\\fragShader.txt");
		//// You can set some simple material properties, these values are passed to the shader
		//// This colour modulates the texture colour
		//modelMaterial->SetDiffuseColour(glm::vec3(1.0f, 1.0f, 1.0f));
		//// The material currently supports one texture
		//// This is multiplied by all the light components (ambient, diffuse, specular)
		//// Note that the diffuse colour set with the line above will be multiplied by the texture colour
		//// If you want just the texture colour, use modelMaterial->SetDiffuseColour( glm::vec3(1,1,1) );
		//modelMaterial->SetTexture(_pwd + "data\\models\\teapot\\TeapotColourMap.bmp");
		//// Need to tell the material the light's position
		//// If you change the light's position you need to call this again
		//modelMaterial->SetLightPosition(m_lightPosition);
		//// Tell the game object to use this material
		//m_model->SetMaterial(modelMaterial);

		// Test PBR Material
		std::shared_ptr<PBRMaterial> material = std::make_shared<PBRMaterial>();
		material->LoadShaders(_pwd + "data\\shaders\\PBRVert.txt", _pwd + "data\\shaders\\PBRFrag.txt");
		material->SetAlbedo(glm::vec3(1,0,0));
		material->SetMetalness(1.0);
		material->SetRoughness(0.2);

		m_model->SetMaterial(material);

		// The mesh is the geometry for the object
		std::shared_ptr<Mesh> modelMesh = std::make_shared<Mesh>();
		// Load from OBJ file. This must have triangulated geometry
		//modelMesh->LoadOBJ(_pwd + "data\\models\\sphere\\triangulated.obj");
		modelMesh->LoadOBJ(_pwd + "data\\models\\teapot\\teapot3.obj");
		// Tell the game object to use this mesh
		m_model->SetMesh(modelMesh);

	}

	Scene::~Scene()
	{
		// You should neatly clean everything up here
	}

	void Scene::Update(float _deltaTs)
	{
		// Update the game object (this is currently hard-coded to rotate)
		m_model->Update(_deltaTs);

		// This updates the camera's position and orientation
		m_viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -2.0f)), m_cameraAngleX, glm::vec3(1, 0, 0)), m_cameraAngleY, glm::vec3(0, 1, 0));
		m_model->m_camPos = glm::inverse(m_viewMatrix) * glm::mat4(1) * glm::vec4(0,0,0,1);
	}

	void Scene::Draw()
	{
		// Draw that model, giving it the camera's position and projection
		m_model->Draw(m_viewMatrix, m_projMatrix);

	}

}