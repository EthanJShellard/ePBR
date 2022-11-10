#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include "Mesh.h"
#include "Material.h"

namespace ePBR 
{

	// The GameObject contains a mesh, a material and position / orientation information
	class GameObject
	{
	public:

		GameObject();
		~GameObject();

		void SetMesh(std::shared_ptr<Mesh> _input) { m_mesh = _input; }
		void SetMaterial(std::shared_ptr<Material> _input) { m_material = _input; }

		void SetPosition(float _posX, float _posY, float _posZ) { m_position.x = _posX; m_position.y = _posY; m_position.z = _posZ; }

		void SetRotation(float _rotX, float _rotY, float _rotZ) { m_rotation.x = _rotX; m_rotation.y = _rotY; m_rotation.z = _rotZ; }
		void SetRotation(glm::vec3 _value) { m_rotation = _value; }
		glm::vec3 GetRotation() { return m_rotation; }


		// Game object is current hard-coded to rotate
		void Update(float _deltaTs);

		// Need to give it the camera's orientation and projection
		void Draw(glm::mat4 _viewMatrix, glm::mat4 _projMatrix);

	protected:

		// The actual model geometry
		std::shared_ptr<Mesh> m_mesh;
		// The material contains the shader
		std::shared_ptr<Material> m_material;

		// Matrix for the position and orientation of the game object
		glm::mat4 m_modelMatrix;
		glm::mat4 m_invModelMatrix;

		// Position of the model
		// The model matrix must be built from this and the _rotation
		glm::vec3 m_position;

		// Orientation of the model
		// The model matrix must be built from this and the _position
		glm::vec3 m_rotation;


	};
}

#endif
