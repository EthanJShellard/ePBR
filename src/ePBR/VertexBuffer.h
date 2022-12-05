#ifndef EPBR_VERTEX_BUFFER
#define EPBR_VERTEX_BUFFER

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


namespace ePBR 
{
	class VertexBuffer 
	{
	public:
		void Add(glm::vec2 _value);
		void Add(float _x, float _y);
		void Add(glm::vec3 _value);
		void Add(float _x, float _y, float _z);
		void Add(glm::vec4 _value);
		void Add(float _x, float _y, float _z, float _w);
		void Add(GLfloat _value);

		void SetData(std::vector<glm::vec3> _newData);
		void SetData(std::vector<glm::vec2> _newData);
		void SetData(std::vector<float> _newData);
		void SetData(const float* _newData, const size_t _size, const int _components);

		//Get the number of components in one unit of data for this buffer. (e.g. 3 for glm::vec3)
		int GetComponents();
		// Get the location of this vertex buffer. Uploads data to GPU if it has changed since last call.
		GLuint GetID();

		VertexBuffer();
		~VertexBuffer();
	private:
		GLuint m_id;
		int m_numComponents;
		std::vector<GLfloat> m_data;
		bool m_dirty; //Used to specify whether data is yet to be uploaded to GPU
	};
}

#endif // EPBR_VERTEX_BUFFER