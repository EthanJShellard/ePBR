
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "GameObject.h"

#include <iostream>

#include "PBRMaterial.h"

namespace ePBR 
{

	GameObject::GameObject() :
		m_position(glm::vec3(0)),
		m_invModelMatrix(glm::mat4(1)),
		m_modelMatrix(glm::mat4(1)),
		m_rotation(glm::vec3(0))
	{
	}

	GameObject::~GameObject()
	{
		// Do any clean up here
	}

	void GameObject::Update(float _deltaTs)
	{
		// Put any update code here
		// Change the _position and _rotation to move the model



	}

	void GameObject::Draw(glm::mat4 _viewMatrix, glm::mat4 _projMatrix)
	{
		if (m_mesh != NULL)
		{
			if (m_material != NULL)
			{

				// Make sure matrices are up to date (if you don't change them elsewhere, you can put this in the update function)
				m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
				m_modelMatrix = glm::rotate(m_modelMatrix, m_rotation.y, glm::vec3(0, 1, 0));
				m_invModelMatrix = glm::rotate(glm::mat4(1.0f), -m_rotation.y, glm::vec3(0, 1, 0));

				// Give all the matrices to the material
				// This makes sure they are sent to the shader
				m_material->SetMatrices(m_modelMatrix, m_invModelMatrix, _viewMatrix, _projMatrix);
				// This activates the shader
				m_material->Apply();
				 
			}
			else if (m_PBRMaterial != NULL) 
			{
				// Make sure matrices are up to date (if you don't change them elsewhere, you can put this in the update function)
				m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
				m_modelMatrix = glm::rotate(m_modelMatrix, m_rotation.y, glm::vec3(0, 1, 0));
				m_invModelMatrix = glm::rotate(glm::mat4(1.0f), -m_rotation.y, glm::vec3(0, 1, 0));

				// Give all the matrices to the material
				// This makes sure they are sent to the shader
				m_PBRMaterial->SetMatrices(m_modelMatrix, m_invModelMatrix, _viewMatrix, _projMatrix);
				// This activates the shader
				m_PBRMaterial->Apply(m_camPos);
			}

			// Sends the mesh data down the pipeline
			m_mesh->Draw();

		}
	}
}
