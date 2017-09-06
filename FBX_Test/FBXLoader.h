#pragma once

#include "fbxsdk.h"

class Mesh;

class FBXLoader
{
public:
	void Init();
	void ImportFBXFile(const char* Path);

	FbxScene const& GetFBXScene() const;

	static FBXLoader& Instance();
private:
	static FBXLoader& _instance;

	void ProcessJointsAndAnimation(Mesh* Mesh);
	void ProcessNode(FbxNode* Node, FbxNode* Parent, unsigned int Index = 0, unsigned int ParentIndex = 0);
	
	FBXLoader();
	~FBXLoader();

	FbxManager*		_fbxManager;
	FbxScene*		_fbxScene;
	FbxIOSettings*	_ioSettings;

	Mesh* _currentMesh;
};