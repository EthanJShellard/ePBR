
#include "Mesh.h"
#include "VertexBuffer.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


namespace ePBR 
{

	Mesh::Mesh()
	{
		// Initialise stuff here
		m_VAO = std::make_shared<VertexArray>();
	}

	Mesh::~Mesh()
	{
	}


	void Mesh::LoadOBJ(std::string _filename)
	{
		// Find file
		std::ifstream inputFile(_filename);

		if (inputFile.is_open())
		{
			// OBJ files can store texture coordinates, positions and normals
			std::vector<glm::vec2> rawUVData;
			std::vector<glm::vec3> rawPositionData;
			std::vector<glm::vec3> rawNormalData;

			std::vector<glm::vec2> orderedUVData;
			std::vector<glm::vec3> orderedPositionData;
			std::vector<glm::vec3> orderedNormalData;
			std::vector<glm::vec3> orderedTangentVectors;
			std::vector <glm::vec3> orderedBitangentVectors;

			std::string currentLine;

			while (std::getline(inputFile, currentLine))
			{
				std::stringstream currentLineStream(currentLine);
				//std::cout<< currentLine <<std::endl;

				if (!currentLine.substr(0, 2).compare(0, 2, "vt"))
				{
					std::string junk;
					float x, y;
					currentLineStream >> junk >> x >> y;
					rawUVData.push_back(glm::vec2(x, y));
				}
				else if (!currentLine.substr(0, 2).compare(0, 2, "vn"))
				{
					std::string junk;
					float x, y, z;
					currentLineStream >> junk >> x >> y >> z;
					rawNormalData.push_back(glm::vec3(x, y, z));
				}
				else if (!currentLine.substr(0, 2).compare(0, 1, "v"))
				{
					std::string junk;
					float x, y, z;
					currentLineStream >> junk >> x >> y >> z;
					rawPositionData.push_back(glm::vec3(x, y, z));
				}
				else if (!currentLine.substr(0, 2).compare(0, 1, "f"))
				{
					std::string junk;
					std::string verts[4];

					currentLineStream >> junk >> verts[0] >> verts[1] >> verts[2] >> verts[3];

					if (verts[3].empty())
					{
						for (unsigned int i = 0; i < 3; i++)
						{
							std::stringstream currentSection(verts[i]);

							// There is just position data
							unsigned int posID = 0;
							unsigned int uvID = 0;
							unsigned int normID = 0;

							if (verts[i].find('/') == std::string::npos)
							{
								// No texcoords or normals
								currentSection >> posID;
							}
							else if (verts[i].find("//") != std::string::npos)
							{
								// No texcoords
								char junk;
								currentSection >> posID >> junk >> junk >> normID;
							}
							else
							{
								char junk;
								currentSection >> posID >> junk >> uvID >> junk >> normID;
							}

							if (posID > 0)
							{
								orderedPositionData.push_back(rawPositionData[posID - 1]);
							}
							if (uvID > 0)
							{
								orderedUVData.push_back(rawUVData[uvID - 1]);
							}
							if (normID > 0)
							{
								orderedNormalData.push_back(rawNormalData[normID - 1]);
							}

						}
					}
					else
					{
						std::cerr << "WARNING: This OBJ loader only works with triangles but a quad has been detected. Please triangulate your mesh." << std::endl;
						inputFile.close();
						return;
					}

				}
			}

			inputFile.close();

			size_t numVertices = orderedPositionData.size();
			m_VAO->SetVertCount(numVertices);

			if (numVertices > 0)
			{
				glBindVertexArray(m_VAO->GetID());

				std::shared_ptr<VertexBuffer> posBuffer = std::make_shared<VertexBuffer>();
				posBuffer->SetData(orderedPositionData);
				m_VAO->SetBuffer(posBuffer, 0);

				if (orderedNormalData.size() > 0)
				{
					std::shared_ptr<VertexBuffer> normalBuffer = std::make_shared<VertexBuffer>();
					normalBuffer->SetData(orderedNormalData);
					m_VAO->SetBuffer(normalBuffer, 1);
				}

				if (orderedUVData.size() > 0)
				{
					std::shared_ptr<VertexBuffer> texBuffer = std::make_shared<VertexBuffer>();
					texBuffer->SetData(orderedUVData);
					m_VAO->SetBuffer(texBuffer, 2);
				}

				if (orderedUVData.size() > 0 && orderedNormalData.size() > 0 && numVertices) 
				{
					orderedTangentVectors.resize(orderedNormalData.size());
					orderedBitangentVectors.resize(orderedNormalData.size());

					for (int i = 0; i < orderedPositionData.size(); i += 3)
					{
						glm::vec3 edge1;
						glm::vec3 edge2;
						glm::vec2 deltaUV1;
						glm::vec2 deltaUV2;
						float f;

						// Vertex 1
						edge1 = orderedPositionData[i + 1] - orderedPositionData[i];
						edge2 = orderedPositionData[i + 2] - orderedPositionData[i];
						deltaUV1 = orderedUVData[i + 1] - orderedUVData[i];
						deltaUV2 = orderedUVData[i + 2] - orderedUVData[i];

						f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

						orderedTangentVectors[i].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
						orderedTangentVectors[i].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
						orderedTangentVectors[i].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
						orderedBitangentVectors[i].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
						orderedBitangentVectors[i].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
						orderedBitangentVectors[i].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

						// Vertex 2
						edge1 = orderedPositionData[i] - orderedPositionData[i + 1];
						edge2 = orderedPositionData[i + 2] - orderedPositionData[i + 1];
						deltaUV1 = orderedUVData[i] - orderedUVData[i + 1];
						deltaUV2 = orderedUVData[i + 2] - orderedUVData[i + 1];

						f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

						orderedTangentVectors[i + 1].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
						orderedTangentVectors[i + 1].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
						orderedTangentVectors[i + 1].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
						orderedBitangentVectors[i + 1].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
						orderedBitangentVectors[i + 1].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
						orderedBitangentVectors[i + 1].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

						// Vertex 3
						edge1 = orderedPositionData[i + 1] - orderedPositionData[i + 2];
						edge2 = orderedPositionData[i] - orderedPositionData[i + 2];
						deltaUV1 = orderedUVData[i + 1] - orderedUVData[i + 2];
						deltaUV2 = orderedUVData[i] - orderedUVData[i + 2];

						f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

						orderedTangentVectors[i + 2].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
						orderedTangentVectors[i + 2].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
						orderedTangentVectors[i + 2].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
						orderedBitangentVectors[i + 2].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
						orderedBitangentVectors[i + 2].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
						orderedBitangentVectors[i + 2].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
					}

					std::shared_ptr<VertexBuffer> tangentBuffer = std::make_shared<VertexBuffer>();
					tangentBuffer->SetData(orderedTangentVectors);
					m_VAO->SetBuffer(tangentBuffer, 3);

					std::shared_ptr<VertexBuffer> bitangentBuffer = std::make_shared<VertexBuffer>();
					bitangentBuffer->SetData(orderedBitangentVectors);
					m_VAO->SetBuffer(bitangentBuffer, 4);
				}
			}
		}
		else
		{
			std::cerr << "WARNING: File not found: " << _filename << std::endl;
		}
	}

	void Mesh::SetAsCube(float _hw) 
	{
		// Makes typing and reading easier. Still represents half-width...
		float w = _hw;

		// Default winding order is counter clockwise
		std::vector<glm::vec3> orderedPositions = 
		{ 
			// Tri 1 of nearest face
			glm::vec3(w, -w, -w), // Bottom right of nearest face
			glm::vec3(-w, -w, -w), // Bottom left of nearest face
			glm::vec3(-w, w, -w), // Top left of nearest face
			// Tri 2 of nearest face
			glm::vec3(-w, w, -w), // Top left of nearest face
			glm::vec3(w, w, -w), // Top right of nearest face
			glm::vec3(w, -w, -w), // Bottom right of nearest face

			// Tri 1 of farthest face (Did these from behind for no reason)
			glm::vec3(-w, w, w), // Top left of farthest face (from behind)
			glm::vec3(-w, -w, w), // Bottom left of farthest face (from behind)
			glm::vec3(w, -w, w), // Bottom right of farthest face (from behind)
			// Tri 2 of farthest face
			glm::vec3(w, -w, w), // Bottom right of farthest face (from behind)
			glm::vec3(w, w, w), // Top right of farthest face (from behind)
			glm::vec3(-w, w, w), // Top left of farthest face (from behind)
			
			// Tri 1 of right hand face
			glm::vec3(w, -w, w), // Bottom right of right hand face
			glm::vec3(w, -w, -w), // Bottom left of right hand face
			glm::vec3(w, w, -w), // Top left of right hand face
			// Tri 2 of right hand face
			glm::vec3(w, w, -w), // Top left of right hand face
			glm::vec3(w, w, w), // Top right of right hand face
			glm::vec3(w, -w, w), // Bottom right of right hand face

			// Tri 1 of left hand face
			glm::vec3(-w, -w, -w), // Bottom right of left hand face
			glm::vec3(-w, -w, w), // Bottom left of left hand face
			glm::vec3(-w, w, w), // Top left of left hand face
			// Tri 2 of left hand face
			glm::vec3(-w, w, w), // Top left of left hand face
			glm::vec3(-w, w, -w), // Top right of left hand face
			glm::vec3(-w, -w, -w), // Bottom right of left hand face

			// Tri 1 of top face
			glm::vec3(w, w, -w), // Bottom right of top face
			glm::vec3(-w, w, -w), // Bottom left of top face
			glm::vec3(-w, w, w), // Top left of top face
			// Tri 2 of top face
			glm::vec3(-w, w, w), // Top left of top face
			glm::vec3(w, w, w), // Top right of top face
			glm::vec3(w, w, -w), // Bottom right of top face

			// Tri 1 of bottom face
			glm::vec3(w, -w, -w), // Top left of bottom face
			glm::vec3(w, -w, w), // Bottom left of bottom face
			glm::vec3(-w, -w, w), // Bottom right of bottom face
			// Tri 2 of bottom face
			glm::vec3(-w, -w, w), // Bottom right of bottom face
			glm::vec3(-w, -w, -w), // Top right of bottom face
			glm::vec3(w, -w, -w) // Top left of bottom face
		};

		std::vector<glm::vec2> orderedUVs = 
		{
			// Tri 1 of nearest face
			glm::vec2(1, 1), // Bottom right of nearest face
			glm::vec2(0, 1), // Bottom left of nearest face
			glm::vec2(0, 0), // Top left of nearest face
			// Tri 2 of nearest face
			glm::vec2(0, 0), // Top left of nearest face
			glm::vec2(1, 0), // Top right of nearest face
			glm::vec2(1, 1), // Bottom right of nearest face

			// Tri 1 of farthest face
			glm::vec2(1, 0), // Top left of farthest face
			glm::vec2(1, 1), // Bottom left of farthest face
			glm::vec2(0, 1), // Bottom right of farthest face
			// Tri 2 of farthest face
			glm::vec2(0, 1), // Bottom right of farthest face
			glm::vec2(0, 0), // Top right of farthest face
			glm::vec2(1, 0), // Top left of farthest face

			// Tri 1 of right hand face
			glm::vec2(1, 1), // Bottom right of right hand face
			glm::vec2(0, 1), // Bottom left of right hand face
			glm::vec2(0, 0), // Top left of right hand face
			// Tri 2 of right hand face
			glm::vec2(0, 0), // Top left of right hand face
			glm::vec2(1, 0), // Top right of right hand face
			glm::vec2(1, 1), // Bottom right of right hand face

			// Tri 1 of left hand face
			glm::vec2(1, 1), // Bottom right of left hand face
			glm::vec2(0, 1), // Bottom left of left hand face
			glm::vec2(0, 0), // Top left of left hand face
			// Tri 2 of left hand face
			glm::vec2(0, 0), // Top left of left hand face
			glm::vec2(1, 0), // Top right of left hand face
			glm::vec2(1, 1), // Bottom right of left hand face

			// Tri 1 of top face
			glm::vec2(1, 1), // Bottom right of top face
			glm::vec2(0, 1), // Bottom left of top face
			glm::vec2(0, 0), // Top left of top face
			// Tri 2 of top face
			glm::vec2(0, 0), // Top left of top face
			glm::vec2(1, 0), // Top right of top face
			glm::vec2(1, 1), // Bottom right of top face

			// Tri 1 of bottom face
			glm::vec2(0, 0), // Top left of bottom face
			glm::vec2(0, 1), // Bottom left of bottom face
			glm::vec2(1, 1), // Bottom right of bottom face
			// Tri 2 of bottom face
			glm::vec2(1, 1), // Bottom right of bottom face
			glm::vec2(1, 0), // Top right of bottom face
			glm::vec2(0, 0) // Top left of bottom face
		};

		std::vector<glm::vec3> orderedNormals = 
		{
			// Tri 1 of nearest face
			glm::vec3(0, 0, -1), // Bottom right of nearest face
			glm::vec3(0, 0, -1), // Bottom left of nearest face
			glm::vec3(0, 0, -1), // Top left of nearest face
			// Tri 2 of nearest face
			glm::vec3(0, 0, -1), // Top left of nearest face
			glm::vec3(0, 0, -1), // Top right of nearest face
			glm::vec3(0, 0, -1), // Bottom right of nearest face

			// Tri 1 of farthest face
			glm::vec3(0, 0, 1), // Top left of farthest face
			glm::vec3(0, 0, 1), // Bottom left of farthest face
			glm::vec3(0, 0, 1), // Bottom right of farthest face
			// Tri 2 of farthest face
			glm::vec3(0, 0, 1), // Bottom right of farthest face
			glm::vec3(0, 0, 1), // Top right of farthest face
			glm::vec3(0, 0, 1), // Top left of farthest face

			// Tri 1 of right hand face
			glm::vec3(1, 0, 0), // Bottom right of right hand face
			glm::vec3(1, 0, 0), // Bottom left of right hand face
			glm::vec3(1, 0, 0), // Top left of right hand face
			// Tri 2 of right hand face
			glm::vec3(1, 0, 0), // Top left of right hand face
			glm::vec3(1, 0, 0), // Top right of right hand face
			glm::vec3(1, 0, 0), // Bottom right of right hand face

			// Tri 1 of left hand face
			glm::vec3(-1, 0, 0), // Bottom right of left hand face
			glm::vec3(-1, 0, 0), // Bottom left of left hand face
			glm::vec3(-1, 0, 0), // Top left of left hand face
			// Tri 2 of left hand face
			glm::vec3(-1, 0, 0), // Top left of left hand face
			glm::vec3(-1, 0, 0), // Top right of left hand face
			glm::vec3(-1, 0, 0), // Bottom right of left hand face

			// Tri 1 of top face
			glm::vec3(0, 1, 0), // Bottom right of top face
			glm::vec3(0, 1, 0), // Bottom left of top face
			glm::vec3(0, 1, 0), // Top left of top face
			// Tri 2 of top face
			glm::vec3(0, 1, 0), // Top left of top face
			glm::vec3(0, 1, 0), // Top right of top face
			glm::vec3(0, 1, 0), // Bottom right of top face

			// Tri 1 of bottom face
			glm::vec3(0, -1, 0), // Bottom right of bottom face
			glm::vec3(0, -1, 0), // Bottom left of bottom face
			glm::vec3(0, -1, 0), // Top left of bottom face
			// Tri 2 of bottom face
			glm::vec3(0, -1, 0), // Top left of bottom face
			glm::vec3(0, -1, 0), // Top right of bottom face
			glm::vec3(0, -1, 0) // Bottom right of bottom face
		};

		// Generated tangent vectors
		std::vector<glm::vec3> orderedTangentVectors =
		{
			glm::vec3(1, -0, -0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, -0, -0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, -0, -0),
			glm::vec3(1, -0, -0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, -0, -0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, -0),
			glm::vec3(-1, -0, -0),
			glm::vec3(-0, -0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(-0, -0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(-0, -0, 1),
			glm::vec3(-0, -0, 1),
			glm::vec3(-0, -0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(-0, -0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(-0, -0, -1),
			glm::vec3(-0, -0, -1),
			glm::vec3(1, -0, -0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, -0, -0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, -0, -0),
			glm::vec3(1, -0, -0),
			glm::vec3(-1, -0, -0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, -0, 0),
			glm::vec3(-1, -0, -0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0)
		};

		// Generated bitangent vectors
		std::vector <glm::vec3> orderedBitangentVectors =
		{
			glm::vec3(0, -1, 0),
			glm::vec3(-0, -1, -0),
			glm::vec3(-0, -1, -0),
			glm::vec3(0, -1, 0),
			glm::vec3(-0, -1, 0),
			glm::vec3(-0, -1, -0),
			glm::vec3(0, -1, 0),
			glm::vec3(-0, -1, -0),
			glm::vec3(-0, -1, -0),
			glm::vec3(-0, -1, 0),
			glm::vec3(0, -1, -0),
			glm::vec3(-0, -1, -0),
			glm::vec3(0, -1, 0),
			glm::vec3(-0, -1, -0),
			glm::vec3(-0, -1, -0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, -0),
			glm::vec3(-0, -1, -0),
			glm::vec3(0, -1, 0),
			glm::vec3(-0, -1, -0),
			glm::vec3(-0, -1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(-0, -1, -0),
			glm::vec3(0, 0, -1),
			glm::vec3(-0, -0, -1),
			glm::vec3(-0, -0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(-0, 0, -1),
			glm::vec3(-0, -0, -1),
			glm::vec3(0, -0, 1),
			glm::vec3(-0, 0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(-0, -0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1)
		};

		m_VAO->SetVertCount(orderedPositions.size());

		std::shared_ptr<VertexBuffer> positionsBuffer = std::make_shared<VertexBuffer>();
		positionsBuffer->SetData(orderedPositions);
		m_VAO->SetBuffer(positionsBuffer, 0);

		std::shared_ptr<VertexBuffer> normalsBuffer = std::make_shared<VertexBuffer>();
		normalsBuffer->SetData(orderedNormals);
		m_VAO->SetBuffer(normalsBuffer, 1);

		std::shared_ptr<VertexBuffer> uvBuffer = std::make_shared<VertexBuffer>();
		uvBuffer->SetData(orderedUVs);
		m_VAO->SetBuffer(uvBuffer, 2);

		std::shared_ptr<VertexBuffer> tangentBuffer = std::make_shared<VertexBuffer>();
		tangentBuffer->SetData(orderedTangentVectors);
		m_VAO->SetBuffer(tangentBuffer, 3);

		std::shared_ptr<VertexBuffer> bitangentBuffer = std::make_shared<VertexBuffer>();
		bitangentBuffer->SetData(orderedBitangentVectors);
		m_VAO->SetBuffer(bitangentBuffer, 4);
	}

	void Mesh::SetAsQuad(float _w, float _h) 
	{
		float w = _w;
		float h = _h;

		std::vector<glm::vec3> orderedPositions =
		{
			// tri 1
			glm::vec3(w, -h, 0), // Bottom right
			glm::vec3(-w, -h, 0), // Bottom left
			glm::vec3(-w, h, 0), // Top left

			// tri 2
			glm::vec3(-w, h, 0), // Top left
			glm::vec3(w, h, 0), // Top right
			glm::vec3(w, -h, 0) // Bottom right
		};

		std::vector<glm::vec3> orderedNormals = 
		{
			// tri 1
			glm::vec3(0, 0, -1), // Bottom right
			glm::vec3(0, 0, -1), // Bottom left
			glm::vec3(0, 0, -1), // Top left

			// tri 2
			glm::vec3(0, 0, -1), // Top left
			glm::vec3(0, 0, -1), // Top right
			glm::vec3(0, 0, -1) // Bottom right
		};

		std::vector<glm::vec2> orderedUVs = 
		{
			// tri 1
			glm::vec2(1, 1), // Bottom right
			glm::vec2(0, 1), // Bottom left
			glm::vec2(0, 0), // Top left
					
			// tri 2
			glm::vec2(0, 0), // Top left
			glm::vec2(1, 0), // Top right
			glm::vec2(1, 1) // Bottom right
		};

		std::vector<glm::vec3> orderedTangentVectors;
		std::vector<glm::vec3> orderedBitangentVectors;

		orderedTangentVectors.resize(orderedNormals.size());
		orderedBitangentVectors.resize(orderedNormals.size());

		for (int i = 0; i < orderedPositions.size(); i += 3)
		{
			glm::vec3 edge1;
			glm::vec3 edge2;
			glm::vec2 deltaUV1;
			glm::vec2 deltaUV2;
			float f;

			// Vertex 1
			edge1 = orderedPositions[i + 1] - orderedPositions[i];
			edge2 = orderedPositions[i + 2] - orderedPositions[i];
			deltaUV1 = orderedUVs[i + 1] - orderedUVs[i];
			deltaUV2 = orderedUVs[i + 2] - orderedUVs[i];

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			orderedTangentVectors[i].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			orderedTangentVectors[i].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			orderedTangentVectors[i].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			orderedBitangentVectors[i].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			orderedBitangentVectors[i].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			orderedBitangentVectors[i].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

			// Vertex 2
			edge1 = orderedPositions[i] - orderedPositions[i + 1];
			edge2 = orderedPositions[i + 2] - orderedPositions[i + 1];
			deltaUV1 = orderedUVs[i] - orderedUVs[i + 1];
			deltaUV2 = orderedUVs[i + 2] - orderedUVs[i + 1];

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			orderedTangentVectors[i + 1].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			orderedTangentVectors[i + 1].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			orderedTangentVectors[i + 1].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			orderedBitangentVectors[i + 1].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			orderedBitangentVectors[i + 1].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			orderedBitangentVectors[i + 1].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

			// Vertex 3
			edge1 = orderedPositions[i + 1] - orderedPositions[i + 2];
			edge2 = orderedPositions[i] - orderedPositions[i + 2];
			deltaUV1 = orderedUVs[i + 1] - orderedUVs[i + 2];
			deltaUV2 = orderedUVs[i] - orderedUVs[i + 2];

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			orderedTangentVectors[i + 2].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			orderedTangentVectors[i + 2].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			orderedTangentVectors[i + 2].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			orderedBitangentVectors[i + 2].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			orderedBitangentVectors[i + 2].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			orderedBitangentVectors[i + 2].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		}

		m_VAO->SetVertCount(orderedPositions.size());

		std::shared_ptr<VertexBuffer> positionsBuffer = std::make_shared<VertexBuffer>();
		positionsBuffer->SetData(orderedPositions);
		m_VAO->SetBuffer(positionsBuffer, 0);

		std::shared_ptr<VertexBuffer> normalsBuffer = std::make_shared<VertexBuffer>();
		normalsBuffer->SetData(orderedNormals);
		m_VAO->SetBuffer(normalsBuffer, 1);

		std::shared_ptr<VertexBuffer> uvBuffer = std::make_shared<VertexBuffer>();
		uvBuffer->SetData(orderedUVs);
		m_VAO->SetBuffer(uvBuffer, 2);

		std::shared_ptr<VertexBuffer> tangentBuffer = std::make_shared<VertexBuffer>();
		tangentBuffer->SetData(orderedTangentVectors);
		m_VAO->SetBuffer(tangentBuffer, 3);

		std::shared_ptr<VertexBuffer> bitangentBuffer = std::make_shared<VertexBuffer>();
		bitangentBuffer->SetData(orderedBitangentVectors);
		m_VAO->SetBuffer(bitangentBuffer, 4);
	}

	void Mesh::Draw()
	{
		// Activate the VAO
		glBindVertexArray(m_VAO->GetID());

		// Tell OpenGL to draw it
		// Must specify the type of geometry to draw and the number of vertices
		glDrawArrays(GL_TRIANGLES, 0, m_VAO->GetVertCount());

		// Unbind VAO
		glBindVertexArray(0);
	}
}
