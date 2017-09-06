
#include "MyWindow.h"
#include "Mesh.h"
#include "Driver.h"
#include "Utilities.h"
#include "FBXLoader.h"
#include "fbxsdk.h"
#include <iostream>
#include "GL\glew.h"

std::vector<Joint> Skeleton;

int FindJointIndexUsingName(const char* Name)
{
	int idx = -1;
	for (Joint joint : Skeleton)
	{ 
		++idx;
		if (joint._name == Name)
			return idx;
	}
	return idx;
}

void ProcessJointsAndAnimation(Mesh* Mesh)
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
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix, transformLinkMatrix, globalBindposeInverseMatrix;
			currCluster->GetTransformMatrix(transformMatrix);
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			Skeleton[currJointIndex]._globalBindposeInverse = globalBindposeInverseMatrix;
			Skeleton[currJointIndex]._node = currCluster->GetLink();

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
			Keyframe** currAnim = &Skeleton[currJointIndex]._animation;

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

void ProcessNode(FbxNode* Node, FbxNode* Parent, unsigned int Index = 0, unsigned int ParentIndex = 0)
{
	if (Parent != nullptr)
	{
		FbxNodeAttribute::EType type = Node->GetNodeAttribute()->GetAttributeType();
		std::cout << "Type "<< type << ": ";

		switch (type)
		{
		case FbxNodeAttribute::eMesh:
		{
			std::cout << "Mesh" << std::endl;
			Mesh* mesh = new (std::nothrow) Mesh;
			mesh->CreateMesh(Node);
		}
		break;
		case FbxNodeAttribute::eSkeleton:
		{	
			std::cout << "Skeleton" << std::endl; 
			Joint currJoint;
			currJoint._name = Node->GetName();
			currJoint._parentIdx = ParentIndex;
			Skeleton.push_back(currJoint);
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
		FbxNode* child = Node->GetChild(i);
		ProcessNode(child, Node, Skeleton.size(), Index);
	}
}

int main(int argc, char* argv[])
{
	Window::Instance().Create("Hello FBX", 1280, 720);
	FBXLoader::Instance().Init();
	FBXLoader::Instance().ImportFBXFile("Resource/Soldier/soldier.fbx");
	//FBXLoader::Instance().ImportFBXFile("Resource/ironman/ironman.fbx");

	FbxNode* rootNode = FBXLoader::Instance().GetFBXScene().GetRootNode();
	ProcessNode(rootNode, nullptr);

	//ProcessJointsAndAnimation(Driver::Instance()._meshes[0]);
	while (Window::Instance().IsOpen())
	{
		Driver::Instance().Clear();
		Driver::Instance().DrawMeshes();

		GLenum err = glGetError();
		if (err != 0)
			std::cout << "gl error: " << err << std::endl;

		Window::Instance().Clear();
	}

	Window::Instance().Close();
	
	return 0;
}