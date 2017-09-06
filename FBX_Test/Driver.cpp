#include "Driver.h"

#define STB_IMAGE_IMPLEMENTATION    
#include "stb\stb_image.h"


#include <iostream>

Driver& Driver::_instance = Driver();

Driver::Driver()
{
}

Driver::~Driver()
{
	while (_meshes.size() > 0)
		delete _meshes[0];
}

void Driver::Init()
{
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		std::cout << "glewInit failed." << std::endl;
	
	glEnable(GL_DEPTH_TEST);
	InitDefaultShader();

}

void Driver::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2, 1, 0.8, 0);
}

void Driver::LoadMesh(Mesh& MeshToLoad)
{
	glGenVertexArrays(1, &MeshToLoad._VAO);
	glBindVertexArray(MeshToLoad._VAO);

	glGenBuffers(1, &MeshToLoad._VBO);
	glBindBuffer(GL_ARRAY_BUFFER, MeshToLoad._VBO);
	glBufferData(GL_ARRAY_BUFFER, MeshToLoad.GetVertices().size() * sizeof(GLfloat), MeshToLoad.GetVertices().data(), GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_meshes.push_back(&MeshToLoad);
}

void Driver::DeleteMesh(Mesh& MeshToDelete)
{
	auto it = std::find(_meshes.begin(), _meshes.end(), &MeshToDelete);
	if (it != _meshes.end())
		_meshes.erase(it);
	else
		std::cout << "Mesh not found.";

	glDeleteVertexArrays(1, &MeshToDelete._VAO);
	glDeleteBuffers(1, &MeshToDelete._VBO);
}

GLuint Driver::LoadTexture(const char* Path, bool Linear)
{
	int w, h, comp;
	unsigned char* image = stbi_load(Path, &w, &h, &comp, STBI_default);
	if (!image)
		std::cout << "Failed to load image." << std::endl;

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (comp == 3) 
		glTexImage2D(GL_TEXTURE_2D, 0, !Linear ? GL_RGB8 : GL_SRGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (comp == 4) 
		glTexImage2D(GL_TEXTURE_2D, 0, !Linear ? GL_RGBA8 : GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	return textureID;
}

void Driver::DeleteTexture(GLsizei Num, GLuint* TextureID)
{
	glDeleteTextures(1, TextureID);
}

void Driver::SendMaterialData(Material const& Mat)
{
	GLint loc = glGetUniformLocation(Mat._shader->GetProgram(), "materialDiffuse");
	if (loc == -1)
		std::cout << "Uniform not found." << std::endl;

	glUniform3f(loc, Mat._diffuse[0], Mat._diffuse[1], Mat._diffuse[2]);

	loc = glGetUniformLocation(Mat._shader->GetProgram(), "materialSpecular");
	if (loc == -1)
	std::cout << "Uniform not found." << std::endl;
	glUniform3f(loc, Mat._specular[0], Mat._specular[1], Mat._specular[2]);
}

void Driver::RenderMesh(Mesh const& MeshToRender)
{
	MeshToRender.GetMatertial()._shader->Bind();
	//SendMaterialData(MeshToRender.GetMatertial());

	glBindVertexArray(MeshToRender._VAO);

	GLint loc = glGetUniformLocation(MeshToRender.GetMatertial()._shader->GetProgram(), "DiffuseTexture");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(MeshToRender.GetMatertial()._shader->GetProgram(), "SpecularTexture");
	glUniform1i(loc, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MeshToRender.GetMatertial()._textureDiffuse);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MeshToRender.GetMatertial()._textureSpecular);

	glDrawArrays(GL_TRIANGLES, 0, MeshToRender.GetVertices().size() / 5);
}

void Driver::DrawMeshes()
{
	for (Mesh* mesh : _meshes)
		RenderMesh(*mesh);
}

void Driver::InitDefaultShader()
{
	_defaultShader.LoadShader(GL_VERTEX_SHADER, "Shader/Shader.vs");
	_defaultShader.LoadShader(GL_FRAGMENT_SHADER, "Shader/Shader.fs");
	_defaultShader.Create();
	_defaultShader.Bind();
}

Driver & Driver::Instance()
{
	return _instance;
}
