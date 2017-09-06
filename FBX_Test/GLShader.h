#ifndef __SHADER_PROGRAM_H_INCLUDED__
#define __SHADER_PROGRAM_H_INCLUDED__

#include <cstdint>

class Shader
{
	public:
		Shader();
		~Shader();
	
		bool LoadShader(uint32_t type, const char* path);
		bool Create();

		void Destroy();
		void Bind();
		void Unbind();

		uint32_t GetProgram();

	private:
		uint32_t		_program;
		uint32_t		_vertexShader;
		uint32_t		_fragmentShader;
		uint32_t		_geometryShader;
};

#endif // __SHADER_PROGRAM_H_INCLUDED__