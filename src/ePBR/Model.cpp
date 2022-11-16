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

		// Load Materials
		std::unordered_map<char*, std::shared_ptr<Texture>> matMap;

		if (scene->HasMaterials()) 
		{
			for (int i = 0; i < scene->mNumMaterials; i++) 
			{
				aiMaterial* material = scene->mMaterials[i];

				// We're only going to load a single texture for the types we want at the moment

				// Diffuse/Albedo
				if (material->GetTextureCount(aiTextureType_DIFFUSE)) 
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
					matMap[path.data] = std::make_shared<Texture>(path.data);
				}

				// Normal map
				if (material->GetTextureCount(aiTextureType_NORMALS)) 
				{
					aiString path;
					material->GetTexture(aiTextureType_NORMALS, 0, &path);
					matMap[path.data] = std::make_shared<Texture>(path.data);
				}

				// Metalness map
				if (material->GetTextureCount(aiTextureType_METALNESS)) 
				{
					aiString path;
					material->GetTexture(aiTextureType_METALNESS, 0, &path);
					matMap[path.data] = std::make_shared<Texture>(path.data);
				}

				// Roughness
				if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path);
					matMap[path.data] = std::make_shared<Texture>(path.data);
				}

				// Ambient occlusion
				if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
				{
					aiString path;
					material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path);
					matMap[path.data] = std::make_shared<Texture>(path.data);
				}
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

				// Find and assign material textures
				std::shared_ptr<PBRMaterial> pbrMaterial = std::make_shared<PBRMaterial>();
				aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
				aiString path;

				// Get diffuse
				if (mat->GetTextureCount(aiTextureType_DIFFUSE)) 
				{
					mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
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