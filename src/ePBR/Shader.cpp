#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "Shader.h"

namespace ePBR
{
	void Shader::LoadNewVertexShader(const char* _path)
	{
		std::ifstream fileRead;
		std::stringstream strStream;
		std::string stringSrc;

		// Detach and delete old vertex shader
		glDetachShader(m_id, m_vertID);
		glDeleteShader(m_vertID);

		fileRead.open(_path);

		if (!fileRead.is_open()) 
		{
			std::cerr << "Failed to open vertex shader at " << _path << std::endl;
			throw std::exception();
		}

		strStream << fileRead.rdbuf();
		stringSrc = strStream.str();
		const char* src = stringSrc.c_str();
		fileRead.close();

		// Create a new vertex shader, attach source code, compile it and
		// check for errors.
		m_vertID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_vertID, 1, &src, NULL);
		glCompileShader(m_vertID);
		GLint success = 0;
		glGetShaderiv(m_vertID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			GLint maxLength = 0;
			glGetShaderiv(m_vertID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(m_vertID, maxLength, &maxLength, &errorLog[0]);
			std::cerr << &errorLog.at(0) << std::endl;
			throw std::exception();
		}

		m_dirty = true;
	}

	void Shader::LoadNewFragmentShader(const char* _path)
	{
		std::ifstream fileRead;
		std::stringstream strStream;
		std::string stringSrc;

		// Detach and delete old fragment shader
		glDetachShader(m_id, m_fragID);
		glDeleteShader(m_fragID);

		fileRead.open(_path);

		if (!fileRead.is_open())
		{
			std::cerr << "Failed to open fragment shader at " << _path << std::endl;
			throw std::exception();
		}

		strStream << fileRead.rdbuf();
		stringSrc = strStream.str();
		const char* src = stringSrc.c_str();
		fileRead.close();

		// Create a new fragment shader, attach source code, compile it and
		// check for errors.
		m_fragID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_fragID, 1, &src, NULL);
		glCompileShader(m_fragID);
		GLint success = 0;
		glGetShaderiv(m_fragID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			GLint maxLength = 0;
			glGetShaderiv(m_fragID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(m_fragID, maxLength, &maxLength, &errorLog[0]);
			std::cerr << &errorLog.at(0) << std::endl;
			throw std::exception();
		}

		m_dirty = true;
	}

	void Shader::BindAttribute(int _index, const char* _identifier)
	{
		glBindAttribLocation(m_id, _index, _identifier);
		m_dirty = true;
	}

	GLuint Shader::GetID()
	{
		if (m_dirty)
		{
			if (!m_id)
			{
				m_id = glCreateProgram();
			}

			GLint success = 0;
			glAttachShader(m_id, m_vertID);
			glAttachShader(m_id, m_fragID);

			// Perform the link and check for failure
			glLinkProgram(m_id);
			glGetProgramiv(m_id, GL_LINK_STATUS, &success);

			if (!success)
			{
				GLint maxLength = 0;
				glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength)
				{
					std::vector<GLchar> errorLog(maxLength);
					glGetProgramInfoLog(m_id, maxLength, &maxLength, &errorLog[0]);
					std::cerr << &errorLog.at(0) << std::endl;
					throw std::exception();
				}
				else
				{
					throw std::exception();
				}
			}

			m_dirty = false;
		}

		return m_id;
	}

	Shader::Shader(const char* _vertexPath, const char* _fragmentPath) :
		m_dirty(true),
		m_vertID(0),
		m_fragID(0),
		m_id(0)
	{
		LoadNewVertexShader(_vertexPath);
		LoadNewFragmentShader(_fragmentPath);

		m_id = glCreateProgram();
	}

	Shader::Shader(const std::string& _vertexPath, const std::string& _fragmentPath) :
		m_dirty(true),
		m_vertID(0),
		m_fragID(0),
		m_id(0)
	{
		LoadNewVertexShader(_vertexPath.c_str());
		LoadNewFragmentShader(_fragmentPath.c_str());

		m_id = glCreateProgram();
	}

	Shader::Shader() :
		m_dirty(true),
		m_vertID(0),
		m_fragID(0),
		m_id(0)
	{
	}

	Shader::~Shader()
	{
		glDetachShader(m_id, m_vertID);
		glDeleteShader(m_vertID);
		glDetachShader(m_id, m_fragID);
		glDeleteShader(m_fragID);
		glDeleteProgram(m_id);
	}
}