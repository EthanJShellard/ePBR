#include "VertexBuffer.h"

#include <stdexcept>

namespace ePBR
{
	void VertexBuffer::Add(glm::vec2 _value)
	{
		if (m_numComponents != 2 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");

		//Flatten data
		m_data.push_back(_value.x);
		m_data.push_back(_value.y);

		m_numComponents = 2;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::Add(float _x, float _y)
	{
		if (m_numComponents != 2 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");;

		m_data.push_back(_x);
		m_data.push_back(_y);

		m_numComponents = 2;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::Add(glm::vec3 _value)
	{
		if (m_numComponents != 3 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");;

		//Flatten data
		m_data.push_back(_value.x);
		m_data.push_back(_value.y);
		m_data.push_back(_value.z);

		m_numComponents = 3;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::Add(float _x, float _y, float _z)
	{
		if (m_numComponents != 3 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");;

		m_data.push_back(_x);
		m_data.push_back(_y);
		m_data.push_back(_z);

		m_numComponents = 3;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::Add(glm::vec4 _value)
	{
		if (m_numComponents != 4 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");;

		//Flatten data
		m_data.push_back(_value.x);
		m_data.push_back(_value.y);
		m_data.push_back(_value.z);
		m_data.push_back(_value.w);

		m_numComponents = 4;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::Add(float _x, float _y, float _z, float _w)
	{
		if (m_numComponents != 4 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");;

		m_data.push_back(_x);
		m_data.push_back(_y);
		m_data.push_back(_z);
		m_data.push_back(_w);

		m_numComponents = 4;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::Add(float _value)
	{
		if (m_numComponents != 1 && m_numComponents != 0) throw std::runtime_error("Incorrect number of components in attempt to add to vertex buffer!");;

		//Flatten data
		m_data.push_back(_value);

		m_numComponents = 1;
		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::SetData(std::vector<glm::vec3> _newData) 
	{
		m_data.clear();
		
		// There may be a way to accelerate this significantly!! This is just quick and dirty for now.
		for (glm::vec3 v : _newData) 
		{
			m_data.push_back(v.x);
			m_data.push_back(v.y);
			m_data.push_back(v.z);
		}

		m_numComponents = 3;
		
		//Data yet to be uploaded
		m_dirty = true;
	}
	void VertexBuffer::SetData(std::vector<glm::vec2> _newData)
	{
		m_data.clear();

		// There may be a way to accelerate this significantly!! This is just quick and dirty for now.
		for (glm::vec2 v : _newData)
		{
			m_data.push_back(v.x);
			m_data.push_back(v.y);
		}

		m_numComponents = 2;

		//Data yet to be uploaded
		m_dirty = true;
	}
	void VertexBuffer::SetData(std::vector<float> _newData)
	{
		m_data.clear();

		// There may be a way to accelerate this significantly!! This is just quick and dirty for now.
		for (float v : _newData)
		{
			m_data.push_back(v);
		}

		m_numComponents = 1;

		//Data yet to be uploaded
		m_dirty = true;
	}

	void VertexBuffer::SetData(const float* _newData, const size_t _size, const int _components)
	{
		// Replace data with new data
		m_data.assign(_newData, _newData + _size);

		// Set components and dirty the object
		m_numComponents = _components;
		m_dirty = true;
	}

	int VertexBuffer::GetComponents()
	{
		return m_numComponents;
	}

	GLuint VertexBuffer::GetID()
	{
		//We know that the data will be needed on the GPU after GETID is called
		//So we upload if the data has changed
		if (m_dirty)
		{
			//Now when we operate, we operate on this buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_id);

			// Upload a copy of the data from memory into the new VBO
			glBufferData(GL_ARRAY_BUFFER,
				m_data.size() * sizeof(m_data.at(0)), //Length of vector times size of datum
				&m_data.at(0), GL_STATIC_DRAW);	//Pointer to first item (data will be contiguous)

			// Reset the state
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Data now uploaded
			m_dirty = false;
		}

		return m_id;
	}

	VertexBuffer::VertexBuffer()
	{
		//Create a new VBO
		glGenBuffers(1, &m_id);
		if (!m_id)
		{
			throw std::exception();
		}

		m_numComponents = 0;
		//Data yet to be uploaded
		m_dirty = true;
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}
}