#pragma once

#include "Material.h"
#include "GL\glew.h"
#include "fbxsdk.h"
#include "Utilities.h"

#include <vector>
#include <unordered_map>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateMesh(FbxNode const* Node);

	int FindJointIndexUsingName(const char* Name);

	std::vector<GLfloat> const& GetVertices() const;
	Material const&				GetMatertial() const;
	FbxNode&					GetFBXNode() const;

	FbxAMatrix GetFinalGlobalTransform(FbxNode& Node);

	GLuint _VAO;
	GLuint _VBO;

	const char* _animationName;
	long        _animationLength;

	Skeleton _skeleton;

	std::unordered_map<unsigned int, ControlPoint*> _controlPoints;
private :
	void FillMaterialData(FbxSurfaceMaterial const& FbxMaterial);


	std::vector<GLfloat>	_vertices;

	FbxNode* _fbxNode;
	Material _material;
};