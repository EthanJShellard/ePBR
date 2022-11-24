#include "Model.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Mesh.h"
#include "PBRMaterial.h"

#include <fstream>
#include <memory>
#include <iostream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ePBR 
{
	void LoadTextureOfType(aiTextureType _type, aiMaterial* _material, std::unordered_map<std::string, std::shared_ptr<Texture>>& _matMap, const std::string& _modelDirectory) 
	{
		if (_material->GetTextureCount(_type))
		{
			aiString path;
			_material->GetTexture(_type, 0, &path);

			if (_matMap.find(path.data) != _matMap.end())
			{
				return;
			}

			// Remove extraneous information at beginning of path
			std::string sPath = path.data;
			if (sPath.at(0) == '.') 
			{
				sPath = sPath.substr(3);
			}
			else if (sPath.at(0) == '\\') 
			{
				sPath = sPath.substr(1);
			}

			std::cout << "Loading " << path.data << std::endl;

			_matMap[path.data] = std::make_shared<Texture>(_modelDirectory + sPath);
		}
	}

	void Model::SetMaterial(int _index, std::shared_ptr<PBRMaterial> _newMat) 
	{
		if (m_materials.size() < _index + 1) 
		{
			m_materials.resize(_index + 1);
		}
		m_materials.at(_index) = _newMat;
	}

	void Model::SetMesh(int _index, std::shared_ptr<Mesh> _newMesh)
	{
		if (m_meshes.size() < _index + 1)
		{
			m_meshes.resize(_index + 1);
		}
		m_meshes.at(_index) = _newMesh;
	}

	void Model::Load(const std::string& _filename)
	{
		std::ifstream fin(_filename.c_str());
		if (!fin.fail())
		{
			fin.close();
		}
		else
		{
			throw std::runtime_error("Failed to open file at " + _filename);
		}

		// Create assimp importer
		Assimp::Importer importer;
		// Load scene from file!
		// Joining identical vertices is breaking things - should be investigated
		const aiScene* scene = importer.ReadFile(_filename, aiProcessPreset_TargetRealtime_Quality ^ aiProcess_JoinIdenticalVertices);

		if (!scene)
		{
			throw std::runtime_error("Import failed from file at " + _filename);
		}

		// Get model location
		std::string locString = _filename.substr(0, _filename.find_last_of('\\') + 1);

		// Load Materials
		std::unordered_map<std::string, std::shared_ptr<Texture>> matMap;

		if (scene->HasMaterials()) 
		{
			for (int i = 0; i < scene->mNumMaterials; i++) 
			{
				aiMaterial* material = scene->mMaterials[i];

				// We're only going to load a single texture for the types we want at the moment
				LoadTextureOfType(aiTextureType_BASE_COLOR, material, matMap, locString);
				LoadTextureOfType(aiTextureType_NORMALS, material, matMap, locString);
				LoadTextureOfType(aiTextureType_METALNESS, material, matMap, locString);
				LoadTextureOfType(aiTextureType_DIFFUSE_ROUGHNESS, material, matMap, locString);
				LoadTextureOfType(aiTextureType_AMBIENT_OCCLUSION, material, matMap, locString);

				std::cout << "Loaded material: " << material->GetName().data << "\n";
			}
			
		}


		if (scene->HasMeshes())
		{
			// Resize vectors
			m_meshes.resize(scene->mNumMeshes);
			m_materials.resize(scene->mNumMeshes);

			for (int meshItr = 0; meshItr < scene->mNumMeshes; ++meshItr)
			{
				std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();

				const aiMesh* mesh = scene->mMeshes[meshItr];

				vao->SetVertCount(mesh->mNumVertices);

				std::cout << "Loading mesh '" << mesh->mName.C_Str() << "':\n";

				// Load vertex positions
				if (mesh->HasPositions()) 
				{
					std::shared_ptr<VertexBuffer> posBuffer = std::make_shared<VertexBuffer>();

					// Cast aiVector3D* to float* doesn't quite work. If you can create an elegant conversion we can do the below
					//posBuffer->SetData((const float *)mesh->mVertices, mesh->mNumVertices, 3);
					
					for (int i = 0; i < mesh->mNumVertices; i++) 
					{
						posBuffer->Add(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
					}

					// Link VBO to VAO
					vao->SetBuffer(posBuffer, 0);

					std::cout << "Loaded " << mesh->mNumVertices << " vertices...\n";
				}

				// Load vertex normals
				if (mesh->HasNormals()) 
				{
					std::shared_ptr<VertexBuffer> normalBuffer = std::make_shared<VertexBuffer>();

					// Cast aiVector3D* to float* doesn't quite work. If you can create an elegant conversion we can do the below
					//normalBuffer->SetData((const float*)mesh->mNormals, mesh->mNumVertices, 3);
					for (int i = 0; i < mesh->mNumVertices; i++)
					{
						normalBuffer->Add(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
					}

					// Link VBO to VAO
					vao->SetBuffer(normalBuffer, 1);

					std::cout << "Loaded " << mesh->mNumVertices << " normals...\n";
				}

				// Load texture coordinates
				if (mesh->mTextureCoords[0]) 
				{
					std::shared_ptr<VertexBuffer> uvBuffer = std::make_shared<VertexBuffer>();

					// Cast aiVector3D* to float* doesn't quite work. If you can create an elegant conversion we can do the below
					//normalBuffer->SetData((const float*)mesh->mNormals, mesh->mNumVertices, 3);
					for (int i = 0; i < mesh->mNumVertices; i++)
					{
						uvBuffer->Add(glm::vec3(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, mesh->mTextureCoords[0][i].z));
					}

					// Link VBO to VAO
					vao->SetBuffer(uvBuffer, 2);

					std::cout << "Loaded " << mesh->mNumVertices << " texture coordinates...\n";
				}

				// Find and assign material textures
				std::shared_ptr<PBRMaterial> pbrMaterial = std::make_shared<PBRMaterial>();
				aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
				aiString path;

				// Get diffuse
				if (mat->GetTextureCount(aiTextureType_BASE_COLOR)) 
				{
					mat->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
					pbrMaterial->SetAlbedoTexture(matMap[path.data]);
				}
				// Get normal
				if (mat->GetTextureCount(aiTextureType_NORMALS))
				{
					mat->GetTexture(aiTextureType_NORMALS, 0, &path);
					pbrMaterial->SetNormalMap(matMap[path.data]);
				}
				// Get metalness
				if (mat->GetTextureCount(aiTextureType_METALNESS))
				{
					mat->GetTexture(aiTextureType_METALNESS, 0, &path);
					pbrMaterial->SetMetalnessMap(matMap[path.data]);
				}
				// Get roughness
				if (mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
				{
					mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path);
					pbrMaterial->SetRoughnessMap(matMap[path.data]);
				}
				// Get diffuse
				if (mat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
				{
					mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path);
					pbrMaterial->SetAmbientOcclusionMap(matMap[path.data]);
				}

				// Instantiate and store mesh
				std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();
				newMesh->SetVertexArray(vao);
				m_meshes.at(meshItr) = newMesh;
				m_materials.at(meshItr) = pbrMaterial;
			}
		}

		// FREE SCENE?
	}

	void Model::Draw(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projMatrix, glm::vec3 _camPos)
	{
		for (int i = 0; i < m_meshes.size(); i++) 
		{
			// This activates and prepares the shader
			m_materials.at(0)->Apply(_modelMatrix, glm::inverse(_modelMatrix), _viewMatrix, _projMatrix, _camPos);

			// Bind vertex arrays and ask openGL to draw
			m_meshes.at(0)->Draw();
		}
	}
}