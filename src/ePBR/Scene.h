
#include "GameObject.h"

// The GLM library contains vector and matrix functions and classes for us to use
// They are designed to easily work with OpenGL!
#include <GLM/glm.hpp> // This is the main GLM header
#include <GLM/gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <GLM/gtc/type_ptr.hpp> // This one gives us access to a utility function which makes sending data to OpenGL nice and easy

#include <string>

namespace ePBR 
{
	class Model;

	// The scene contains objects, the camera and light
	// It is responsible for coordinating these things
	class Scene
	{
	public:


		// Currently the scene is set up in the constructor
		// This means the object(s) are loaded, given materials and positions as well as the camera and light
		Scene(const std::string& _pwd);
		~Scene();

		// Use these to adjust the camera's orientation
		// Camera is currently set up to rotate about the world-space origin NOT the camera's origin
		void ChangeCameraAngleX(float _value) { m_cameraAngleX += _value; }
		void ChangeCameraAngleY(float _value) { m_cameraAngleY += _value; }

		// Calls update on all objects in the scene
		void Update(float _deltaTs);

		// Draws the scene from the camera's point of view
		void Draw();

		std::shared_ptr<GameObject> GetObject() { return m_model; }
		std::shared_ptr<Model> GetModel() { return m_testModel; }
	protected:

		// Currently one object, this could be a list of objects!
		std::shared_ptr<GameObject> m_model;
		std::shared_ptr<Model> m_testModel;

		// This matrix represents the camera's position and orientation
		glm::mat4 m_viewMatrix;

		// This matrix is like the camera's lens
		glm::mat4 m_projMatrix;

		// Current rotation information about the camera
		float m_cameraAngleX, m_cameraAngleY;

		// Position of the single point-light in the scene
		glm::vec3 m_lightPosition;

		// Position of the camera
		glm::vec3 m_camPos;


	};
}