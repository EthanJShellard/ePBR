#ifndef EPBR_VERTEX_ARRAY
#define EPBR_VERTEX_ARRAY

#include <memory>
#include <vector>

#include <GL/glew.h>

namespace ePBR
{
	class VertexBuffer;

	/// @brief Wrapper for vertex array objects. Will bind buffers lazily, when GetID() is called.
	class VertexArray
	{
	public:
		
		/// @brief Set a vertex buffer on this vertex array.
		/// @param _buffer The new buffer.
		/// @param _index The index at which the new buffer will be placed.
		void SetBuffer(std::shared_ptr<VertexBuffer> _buffer, int _index);

		/// @brief Get the OpenGL ID of this vertex array.
		/// @return The ID.
		GLuint GetID();

		/// @brief Get the number of vertices associated with this vertex array.
		/// @return The vertex count.
		unsigned int GetVertCount() const;

		/// @brief Set the number of vertices assoiciated with this vertex array.
		/// @param _count The new vertex count.
		void SetVertCount(unsigned int _count);

		VertexArray();
		~VertexArray();
	private:
		GLuint m_id;
		unsigned int m_vertCount;
		bool m_dirty;
		std::vector< std::shared_ptr<VertexBuffer> > m_buffers;
	};
}

#endif // EPBR_VERTEX_ARRAY