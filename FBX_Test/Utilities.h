#pragma once

#include "fbxsdk.h"

#include <vector>

struct Keyframe
{
	FbxLongLong _frameNum;
	FbxAMatrix _globalTransform;
	Keyframe* _next;

	Keyframe() :_next(nullptr) {}
};

struct BlendingIndexWeightPair
{
	unsigned int	_blendingIdx = 0;
	double			_blendingWeight = 0;
};

struct Joint
{
	int			_parentIdx = -1;
	const char*	_name = "";
	FbxAMatrix	_globalBindposeInverse;
	FbxNode*	_node = nullptr;
	Keyframe*	_animation;
};

struct ControlPoint
{
	FbxDouble3 _pos;
	std::vector<BlendingIndexWeightPair> _blendingInfo;
};

struct Skeleton
{
	std::vector<Joint> _joints;
};