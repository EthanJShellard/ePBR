
#ifndef EPBR_MESH
#define EPBR_MESH

#include "VertexArray.h"

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include <memory>

namespace ePBR 
{
	// For loading a mesh from OBJ file and keeping a reference for it
	class Mesh
	{
	public:

		Mesh();
		~Mesh();

		// OBJ file must be triangulated
		void LoadOBJ(std::string _filename);

		void SetVertexArray(std::shared_ptr<VertexArray> _newVAO) { m_VAO = _newVAO; };

		// Draws the mesh - must have shaders applied for this to display!
		void Draw();

	protected:

		// OpenGL Vertex Array Object
		std::shared_ptr<VertexArray> m_VAO;
	};
}
#endif // EPBR_MESH
