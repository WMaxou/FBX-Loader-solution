#include "GLShader.h"

#include "GL/glew.h"

#include <fstream>
#include <iostream>

uint32_t Shader::GetProgram()
{
	return _program;
}

Shader::Shader()
{
	_program = 0;
	_vertexShader = 0;
	_fragmentShader = 0;
	_geometryShader = 0;
}

Shader::~Shader()
{
//	delete &_program;
//	delete &_vertexShader;
//	delete &_fragmentShader;
//	delete &_geometryShader;
}

bool Shader::LoadShader(uint32_t type, const char* path)
{
	uint32_t shaderID = glCreateShader(type);

	switch (type)
	{
		case GL_VERTEX_SHADER:
			_vertexShader = shaderID;
			break;

		case GL_FRAGMENT_SHADER:
			_fragmentShader = shaderID;
			break;

		case GL_GEOMETRY_SHADER:
			_geometryShader = shaderID;
			break;

		default: break;
	}
	
	std::ifstream fileStream;
	fileStream.open(path, std::ios::binary);
	if (!fileStream.is_open())
		return false;

	auto begin = fileStream.tellg();
	fileStream.seekg(0, std::ios::end);
	auto len = fileStream.tellg() - begin;
	char* buffer = new char[(len + 1)];
	fileStream.seekg(std::ios::beg);
	fileStream.read(buffer, len);
	buffer[len] = '\0';

	glShaderSource(shaderID, 1, &buffer, nullptr);
	delete[] buffer;
	fileStream.close();

	glCompileShader(shaderID);

	int compileSuccess = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);

	if (!compileSuccess)
	{
		char errorBuffer[4096];
		int errorLen;
		glGetShaderInfoLog(shaderID, 4096, &errorLen, errorBuffer);
		if (type == GL_VERTEX_SHADER)
			std::cout << "Error dans le " << "vertex shader: " << errorBuffer << std::endl;
		if (type == GL_FRAGMENT_SHADER)
			std::cout << "Error dans le " << "fragment shader: " << errorBuffer << std::endl;
		if (type == GL_GEOMETRY_SHADER)
			std::cout << "Error dans le " << "geometry shader: " << errorBuffer << std::endl;
		return false;
	}

	return true;
}

void Shader::Destroy()
{
	glDeleteShader(_vertexShader);
	glDeleteShader(_fragmentShader);
	glDeleteShader(_geometryShader);
	glDeleteProgram(_program);
}

bool Shader::Create()
{
	_program = glCreateProgram();

	glAttachShader(_program, _vertexShader);
	glAttachShader(_program, _fragmentShader);
	if (_geometryShader != 0)
		glAttachShader(_program, _geometryShader);
	glLinkProgram(_program);

	int linkSuccess;
	glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);

	if (!linkSuccess)
	{
		char errorBuffer[4096];
		int errorLen;
		glGetProgramInfoLog(_program, 4096, &errorLen, errorBuffer);
		std::cout << "Erreur linkage " << errorBuffer << std::endl;
		return false;
	}

	return true;
}

void Shader::Bind()
{ 
	glUseProgram(_program);
}

void Shader::Unbind()
{ 
	glUseProgram(0); 
}