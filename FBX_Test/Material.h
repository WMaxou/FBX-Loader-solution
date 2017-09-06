#pragma once

#include "GL\glew.h"

class Shader;

struct Material
{
	float	_diffuse[3] = { 0.f, 0.f, 0.f };
	float	_specular[3] = { 0.f, 0.f, 0.f };

	GLuint	_textureDiffuse = 0;
	GLuint	_textureSpecular = 0;
	Shader* _shader = nullptr;
};
