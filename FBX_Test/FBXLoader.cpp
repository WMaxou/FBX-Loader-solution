#include "FBXLoader.h"
#include "Utilities.h"
#include "Mesh.h"

#include <iostream>

FBXLoader& FBXLoader::_instance = FBXLoader();

void FBXLoader::Init()
{
	_fbxManager = FbxManager::Create();
	_ioSettings = FbxIOSettings::Create(_fbxManager, IOSROOT);
	_fbxManager->SetIOSettings(_ioSettings);

	_fbxScene = FbxScene::Create(_fbxManager, "My Scene");
}

void FBXLoader::ImportFBXFile(const char* Path)
{
	FbxImporter* _fbxImporter = FbxImporter::Create(_fbxManager, "My Importer");
	bool status = _fbxImporter->Initialize(Path, -1, _fbxManager->GetIOSettings());

	if (!status)
		std::cout << "Failed to initialize fbx file." << std::endl;

	status = _fbxImporter->Import(_fbxScene);
	if (!status)
		std::cout << "Failed to import fbx file." << std::endl;
	
	_fbxImporter->Destroy();
}

FbxScene const & FBXLoader::GetFBXScene() const
{
	return *_fbxScene;
}

FBXLoader& FBXLoader::Instance()
{
	return _instance;
}

FBXLoader::FBXLoader()
{
	_currentMesh = nullptr;
}

FBXLoader::~FBXLoader()
{
	_fbxScene->Destroy();
	_fbxManager->Destroy();
}


void FBXLoader::ProcessJointsAndAnimation(Mesh* Mesh)
{
	FbxMesh* currFbxMesh = Mesh->GetFBXNode().GetMesh();
	unsigned int deformersCount = currFbxMesh->GetDeformerCount();

	FbxAMatrix geometryTransform = Mesh->GetFinalGlobalTransform(Mesh->GetFBXNode());

	for (unsigned int deformerId = 0; deformerId < deformersCount; ++deformerId)
	{
		FbxSkin* currSkin = (FbxSkin*)currFbxMesh->GetDeformer(deformerId, FbxDeformer::eSkin);
		if (currSkin == nullptr)
			continue;

		unsigned int clusterCount = currSkin->GetClusterCount();
		for (unsigned int clusterId = 0; clusterId < clusterCount; ++clusterId)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterId);
			const char* currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = Mesh->FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix, transformLinkMatrix, globalBindposeInverseMatrix;
			currCluster->GetTransformMatrix(transformMatrix);
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			Mesh->_skeleton._joints[currJointIndex]._globalBindposeInverse = globalBindposeInverseMatrix;
			Mesh->_skeleton._joints[currJointIndex]._node = currCluster->GetLink();

			unsigned int indiceCount = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < indiceCount; ++i)
			{
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair._blendingIdx = currJointIndex;
				currBlendingIndexWeightPair._blendingWeight = currCluster->GetControlPointWeights()[i];
				Mesh->_controlPoints[currCluster->GetControlPointIndices()[i]]->_blendingInfo.push_back(currBlendingIndexWeightPair);
			}

			FbxAnimStack* currAnimStack = FBXLoader::Instance().GetFBXScene().GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			Mesh->_animationName = animStackName;
			FbxTakeInfo* takeInfo = FBXLoader::Instance().GetFBXScene().GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			Mesh->_animationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			Keyframe** currAnim = &Mesh->_skeleton._joints[currJointIndex]._animation;

			for (long i = start.GetFrameCount(FbxTime::eFrames24); i < end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->_frameNum = i;
				FbxAMatrix currentTransformOffset = Mesh->GetFBXNode().EvaluateGlobalTransform(currTime) * geometryTransform;
				(*currAnim)->_globalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->_next);
			}
		}
	}
	BlendingIndexWeightPair currBlendingIndexWeightPair;
	for (auto itr = Mesh->_controlPoints.begin(); itr != Mesh->_controlPoints.end(); ++itr)
	{
		for (unsigned int i = itr->second->_blendingInfo.size(); i <= 4; ++i)
		{
			itr->second->_blendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

void FBXLoader::ProcessNode(FbxNode* Node, FbxNode* Parent, unsigned int Index, unsigned int ParentIndex)
{
	if (Parent != nullptr)
	{
		FbxNodeAttribute::EType type = Node->GetNodeAttribute()->GetAttributeType();
		std::cout << "Type " << type << ": ";

		switch (type)
		{
		case FbxNodeAttribute::eMesh:
		{
			std::cout << "Mesh" << std::endl;
			Mesh* mesh = new (std::nothrow) Mesh;
			mesh->CreateMesh(Node);
			_currentMesh = mesh;
		}
		break;
		case FbxNodeAttribute::eSkeleton:
		{
			std::cout << "Skeleton" << std::endl;
			Joint currJoint;
			currJoint._name = Node->GetName();
			currJoint._parentIdx = ParentIndex;
			_currentMesh->_skeleton._joints.push_back(currJoint);
		}
		break;

		default: std::cout << "not handled." << std::endl;
			break;
		}
	}

	unsigned int childCount = Node->GetChildCount();
	std::cout << "Child count: " << childCount << std::endl;
	for (unsigned int i = 0; i < childCount; ++i)
	{
		
		unsigned int size = 0;
		if (_currentMesh != nullptr)
			size = _currentMesh->_skeleton._joints.size();

		FbxNode* child = Node->GetChild(i);
		ProcessNode(child, Node, size, Index);
	}
}