#include <memory>
#include <vector>

#include <GL/glew.h>

namespace ePBR
{
	//Forward Declaration
	class VertexBuffer;

	//Wrapper for vertex array objects. Will bind buffers lazily, when GetID() is called.
	class VertexArray
	{
	public:
		/// <summary>
		/// Set a vertex buffer on this VBO. This will make the Vertex Array dirty
		/// </summary>
		void SetBuffer(std::shared_ptr<VertexBuffer> _buffer, int _position);
		//Bind buffers if they've changed and then return ID
		GLuint GetID();
		size_t GetVertCount();
		void SetVertCount(size_t _count);

		VertexArray();
		~VertexArray();
	private:
		GLuint m_id;
		size_t m_vertCount;
		bool m_dirty;
		std::vector< std::shared_ptr<VertexBuffer> > m_buffers;
	};
}