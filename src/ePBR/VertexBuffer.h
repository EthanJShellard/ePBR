#ifndef EPBR_VERTEX_BUFFER
#define EPBR_VERTEX_BUFFER

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


namespace ePBR 
{
	/// @brief Wrapper for OpenGL vertex buffers. Will upload data lazily.
	class VertexBuffer 
	{
	public:
		/// @brief Add a 2 dimensional value to this buffer.
		/// @param _value The value.
		void Add(glm::vec2 _value);

		/// @brief Add a 2 dimensional value to this buffer.
		/// @param _x The first dimension.
		/// @param _y The second dimension.
		void Add(float _x, float _y);

		/// @brief Add a 3 dimensional value to this buffer.
		/// @param _value The value.
		void Add(glm::vec3 _value);

		/// @brief Add a 3 dimensional value to this buffer.
		/// @param _x The first dimension.
		/// @param _y The second dimension.
		/// @param _z The third dimension.
		void Add(float _x, float _y, float _z);

		/// @brief Add a 4 dimensional value to this buffer.
		/// @param _value The value.
		void Add(glm::vec4 _value);

		/// @brief Add a 3 dimensional value to this buffer.
		/// @param _x The first dimension.
		/// @param _y The second dimension.
		/// @param _z The third dimension.
		/// @param _w The fourth dimension.
		void Add(float _x, float _y, float _z, float _w);

		/// @brief Add a 1 dimensional value to this buffer
		/// @param _value The value.
		void Add(float _value);

		/// @brief Replace the data in this buffer with new, 3 dimensional data.
		/// @param _newData The new data.
		void SetData(std::vector<glm::vec3> _newData);

		/// @brief Replace the data in this buffer with new, 2 dimensional data.
		/// @param _newData The new data.
		void SetData(std::vector<glm::vec2> _newData);

		/// @brief Replace the data in this buffer with new, 1 dimensional data.
		/// @param _newData The new data.
		void SetData(std::vector<float> _newData);

		/// @brief Replace the data in this buffer with a raw float array.
		/// @param _newData A pointer to the start of the new data.
		/// @param _size The number of elements in _newData.
		/// @param _components The number of dimenions (i.e. 3 for vec3s) expected in the data.
		void SetData(const float* _newData, const size_t _size, const int _components);

		/// @brief Get the number of components in one unit of data for this buffer. (e.g. 3 for vec3)
		/// @return The number of components in one unit of data for this buffer.
		int GetComponents();

		/// @brief Get the OpenGL ID of this buffer.
		/// @return The ID.
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