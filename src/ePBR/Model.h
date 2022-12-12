#ifndef EPBR_MODEL
#define EPBR_MODEL

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace ePBR 
{
	class PBRMaterial;
	class Mesh;

	class Model 
	{
	private:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<PBRMaterial>> m_materials;

	public:
		/// @brief Get this Model's meshes.
		/// @return A vector containing this Model's meshes
		std::vector<std::shared_ptr<Mesh>> GetMeshes() { return m_meshes; }

		/// @brief Get this Model's materials.
		/// @return A vector containing this Model's materials.
		std::vector<std::shared_ptr<PBRMaterial>> GetMaterials() { return m_materials; }

		/// @brief Set a material at a specified index on this Model.
		/// @param _index The index of the material to be set.
		/// @param _newMat The new material.
		void SetMaterial(int _index, std::shared_ptr<PBRMaterial> _newMat);

		/// @brief Set a Mesh at a specified index on this Model.
		/// @param _index The index of the Mesh to be set.
		/// @param _newMesh The new Mesh.
		void SetMesh(int _index, std::shared_ptr<Mesh> _newMesh);

		/// @brief Load a model from a file.
		/// @param _filename The path to the model to load.
		void Load(const std::string& _filename);

		/// @brief Draw a model.
		/// @param _modelMatrix The model matrix.
		/// @param _viewMatrix The view matrix.
		/// @param _projMatrix The projection matrix.
		/// @param _camPos The position of the camera.
		void Draw(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos);
	};
}

#endif EPBR_MODEL