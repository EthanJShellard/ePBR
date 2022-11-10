
#include "Mesh.h"
#include "VertexBuffer.h"

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


	void Mesh::LoadOBJ(std::string m_filename)
	{
		// Find file
		std::ifstream inputFile(m_filename);

		if (inputFile.is_open())
		{
			// OBJ files can store texture coordinates, positions and normals
			std::vector<glm::vec2> rawUVData;
			std::vector<glm::vec3> rawPositionData;
			std::vector<glm::vec3> rawNormalData;

			std::vector<glm::vec2> orderedUVData;
			std::vector<glm::vec3> orderedPositionData;
			std::vector<glm::vec3> orderedNormalData;

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
			}
		}
		else
		{
			std::cerr << "WARNING: File not found: " << m_filename << std::endl;
		}
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
