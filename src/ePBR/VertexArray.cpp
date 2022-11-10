#include "VertexArray.h"
#include "VertexBuffer.h"

namespace ePBR
{
	void VertexArray::SetBuffer(std::shared_ptr<VertexBuffer> _buffer, int _position)
	{
		if (m_buffers.size() <= _position) m_buffers.resize(_position + 1);
		m_buffers.at(_position) = _buffer;
		m_dirty = true; //Data has changed and so needs to be uploaded
	}

	GLuint VertexArray::GetID()
	{
		if (m_dirty)
		{
			glBindVertexArray(m_id);

			for (size_t i = 0; i < m_buffers.size(); i++)
			{
				if (!m_buffers.at(i)) continue; //If null go to next

				glBindBuffer(GL_ARRAY_BUFFER, m_buffers.at(i)->GetID());
				glVertexAttribPointer(i, m_buffers.at(i)->GetComponents(), GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(i);
			}

			//cleanup
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_dirty = false;
		}

		return m_id;
	}

	size_t VertexArray::GetVertCount()
	{
		return m_vertCount;
	}

	void VertexArray::SetVertCount(size_t _count)
	{
		m_vertCount = _count;
	}

	VertexArray::VertexArray()
	{
		// Create a new VAO on the GPU and bind it
		glGenVertexArrays(1, &m_id);
		if (!m_id)
		{
			throw std::exception();
		}

		m_buffers.resize(3); //to save on resizes. Our default size will be 3

		m_vertCount = 0;
		m_dirty = true;
	}

	VertexArray::~VertexArray()
	{
		//Buffers should delete due to RAII
		glDeleteVertexArrays(1, &m_id);
	}
}