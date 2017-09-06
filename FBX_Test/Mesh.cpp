#include "Mesh.h"
#include "Driver.h"

#include <iostream>

Mesh::Mesh()
{
	_material._shader = &Driver::Instance()._defaultShader;
}

Mesh::~Mesh()
{
	Driver::Instance().DeleteMesh(*this);

	_vertices.clear();
}

void Mesh::CreateMesh(FbxNode const* Node)
{
	FbxNode* casted = const_cast<FbxNode*>(Node);
	FbxMesh* mesh = casted->GetMesh();

	_fbxNode = casted;

	size_t polygonCount = mesh->GetPolygonCount();

	FbxAMatrix finalGlobalTransform = GetFinalGlobalTransform(*casted);

	FbxStringList uvChannelNames;
	mesh->GetUVSetNames(uvChannelNames);
	int uvChannelCount = uvChannelNames.GetCount();
	for (unsigned int p = 0; p < polygonCount; ++p)
	{
		for (unsigned int v = 0; v < 3; ++v)
		{

			int index = mesh->GetPolygonVertex(p, v);
			FbxVector4 pos = mesh->mControlPoints[index];
			pos = finalGlobalTransform.MultT(pos);
			_vertices.push_back((GLfloat)pos.mData[0]);
			_vertices.push_back((GLfloat)pos.mData[1]);
			_vertices.push_back((GLfloat)pos.mData[2]);

			char* uvChannel = uvChannelNames.GetStringAt(0);
			FbxVector2 texCoords;
			bool mapped = false;
			mesh->GetPolygonVertexUV(p, v, uvChannel, texCoords, mapped);

			_vertices.push_back(texCoords.mData[0]);
			_vertices.push_back(texCoords.mData[1]);
		}
	}

	FillMaterialData(*Node->GetMaterial(0));

	Driver::Instance().LoadMesh(*this);
	std::cout << "Mesh created !" << std::endl;
}

void Mesh::FillMaterialData(FbxSurfaceMaterial const& FbxMaterial)
{
	const FbxProperty propertyDiffuse = FbxMaterial.FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxDouble3	color = propertyDiffuse.Get<FbxDouble3>();

	FbxFileTexture* texture = propertyDiffuse.GetSrcObject<FbxFileTexture>(0);
	if (texture)
	{
		const char* fileName = texture->GetFileName();
		_material._textureDiffuse = Driver::Instance().LoadTexture(fileName, true);
	}

	const FbxProperty factorDiffuse = FbxMaterial.FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	double factor = factorDiffuse.Get<double>();
	_material._diffuse[0] = (float)(color[0] * factor);
	_material._diffuse[1] = (float)(color[1] * factor);
	_material._diffuse[2] = (float)(color[2] * factor);

	const FbxProperty propertySpecular = FbxMaterial.FindProperty(FbxSurfaceMaterial::sSpecular);
	color = propertySpecular.Get<FbxDouble3>();

	texture = propertySpecular.GetSrcObject<FbxFileTexture>();
	if (texture)
	{
		const char* fileName = texture->GetFileName();
		_material._textureSpecular = Driver::Instance().LoadTexture(fileName, true);
	}

	FbxProperty factorSpecular = FbxMaterial.FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	factor = factorSpecular.Get<double>();
	_material._specular[0] = (float)(color[0] * factor);
	_material._specular[1] = (float)(color[1] * factor);
	_material._specular[2] = (float)(color[2] * factor);
}

FbxAMatrix Mesh::GetFinalGlobalTransform(FbxNode& Node)
{
	FbxAMatrix globalTransform = Node.GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(&Node);
	FbxVector4 translation = Node.GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rotation = Node.GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scale = Node.GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryTransform;
	geometryTransform.SetTRS(translation, rotation, scale);

	return globalTransform * geometryTransform;
}

std::vector<GLfloat> const & Mesh::GetVertices() const
{
	return _vertices;
}

Material const & Mesh::GetMatertial() const
{
	return _material;
}

FbxNode& Mesh::GetFBXNode() const
{
	return *_fbxNode;
}

int Mesh::FindJointIndexUsingName(const char* Name)
{
	int idx = -1;
	for (Joint joint : _skeleton._joints)
	{
		++idx;
		if (joint._name == Name)
			return idx;
	}
	return idx;
}
