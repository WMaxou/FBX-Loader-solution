#pragma once

#include "Mesh.h"
#include "GLShader.h"
#include "GL\glew.h"

#include <vector>

class Driver
{
public:
	void Init(); // after opengl context
	void Clear();

	void	LoadMesh(Mesh& MeshToLoad);
	void	DeleteMesh(Mesh& MeshToDelete);
	GLuint	LoadTexture(const char* Path, bool Linear = false);
	void	DeleteTexture(GLsizei Num, GLuint* TextureID);

	void	SendMaterialData(Material const& Mat);

	void	RenderMesh(Mesh const& MeshToRender);
	void	DrawMeshes();

	Shader _defaultShader;
	std::vector<Mesh*> _meshes;

	static Driver& Instance();
private:
	Driver();
	~Driver();

	static Driver& _instance;
	
	void InitDefaultShader();
};