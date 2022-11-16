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
		std::vector<std::shared_ptr<Mesh>> GetMeshes() { return m_meshes; }
		std::vector<std::shared_ptr<PBRMaterial>> GetMaterials() { return m_materials; }

		void SetMaterial(int _index, std::shared_ptr<PBRMaterial> _newMat) { m_materials.at(_index) = _newMat; }
		void SetMesh(int _index, std::shared_ptr<Mesh> _newMesh) { m_meshes.at(_index) = _newMesh; }

		void Load(const std::string& _filename);
		void Draw(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos);
	};
}

#endif EPBR_MODEL