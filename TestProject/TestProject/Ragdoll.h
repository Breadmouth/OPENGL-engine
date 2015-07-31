#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

using namespace physx;

enum RagdollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
};

const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);

struct RagdollNode
{
	//rotation of this link in model space
	PxQuat globalRotation;
	//position of the link in world space
	PxVec3 scaledGlobalPos;

	//index of parent node
	int parentNodeIdx;
	//half length of the capsule for this node
	float halfLength;
	//radius of capsule for this node
	float radius;
	//relative position of link centre in parent node
	float parentLinkPos;
	//relative position of link centre in child node
	float childLinkPos;
	//name of link
	char* name;
	PxArticulationLink* linkPtr;

	RagdollNode(PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float _radius, float _parentLinkPos, float _childLinkPos, char* _name)
	{
		globalRotation = _globalRotation, parentNodeIdx = _parentNodeIdx, halfLength = _halfLength, radius = _radius, parentLinkPos = _parentLinkPos, childLinkPos = _childLinkPos, name = _name;
	};
};

static RagdollNode* ragdollData[] =
{
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NO_PARENT, 1, 3, 1, 1, "lower spine"),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), LOWER_SPINE, 1, 1, -1, 1, "left pelvis"),
	new RagdollNode(PxQuat(0, Z_AXIS), LOWER_SPINE, 1, 1, -1, 1, "right pelvis"),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_PELVIS, 5, 2, -1, 1, "L upper leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_PELVIS, 5, 2, -1, 1, "R upper leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_UPPER_LEG, 5, 1.75, -1, 1, "L lower leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_UPPER_LEG, 5, 1.75, -1, 1, "R lowerleg"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), LOWER_SPINE, 1, 3, 1, -1, "upper spine"),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "left clavicle"),
	new RagdollNode(PxQuat(0, Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "right clavicle"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), UPPER_SPINE, 1, 1, 1, -1, "neck"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NECK, 1, 3, 1, -1, "HEAD"),
	new RagdollNode(PxQuat(PxPi - .3, Z_AXIS), LEFT_CLAVICLE, 3, 1.5, -1, 1, "left upper arm"),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_CLAVICLE, 3, 1.5, -1, 1, "right upper arm"),
	new RagdollNode(PxQuat(PxPi - .3, Z_AXIS), LEFT_UPPER_ARM, 3, 1, -1, 1, "left lower arm"),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_UPPER_ARM, 3, 1, -1, 1, "right lower arm"),
	NULL
};

class Ragdoll : public PxArticulation
{
public:
	Ragdoll();
	~Ragdoll();
	static PxArticulation* MakeRagdoll(PxPhysics* g_physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial);
};

