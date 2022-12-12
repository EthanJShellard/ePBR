
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
	class Mesh
	{
	public:

		Mesh();
		~Mesh();

		/// @brief Load a mesh from a wavefront OBJ file. The mesh must be triangulated.
		/// @param _filename The path to the OBJ mesh to import.
		void LoadOBJ(std::string _filename);

		/// @brief Set this mesh to a uniform cube.
		/// @param _halfWidth Half of the desired cube's width.
		void SetAsCube(float _halfWidth);

		/// @brief Set this mesh to a quad.
		/// @param _halfWidth Half the width of the desired quad.
		/// @param _halfHeight Half the height of the desired quad.
		void SetAsQuad(float _halfWidth, float _halfHeight);

		/// @brief Swap out the vertex array of this Mesh.
		/// @param _newVAO The new vertex array this mesh will use.
		void SetVertexArray(std::shared_ptr<VertexArray> _newVAO) { m_VAO = _newVAO; };

		/// @brief Bind and draw this mesh. Does not apply any material or shader.
		void Draw();

	protected:

		// OpenGL Vertex Array Object
		std::shared_ptr<VertexArray> m_VAO;
	};
}
#endif // EPBR_MESH
