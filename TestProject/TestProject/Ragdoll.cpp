#include "Ragdoll.h"

PxArticulation* Ragdoll::MakeRagdoll(PxPhysics* g_physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial)
{
	//create articulation for our ragdoll
	PxArticulation* articulation = g_physics->createArticulation();
	RagdollNode** currentNode = nodeArray;

	while (*currentNode != NULL)
	{
		//get a pointer to the current node
		RagdollNode* currentNodePtr = *currentNode;
		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		//get scaled values for the capsule
		float radius = currentNodePtr->radius * scaleFactor;
		float halfLength = currentNodePtr->halfLength * scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0;
		//get a pointer to the parent
		PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGlobalPos = worldPos.p;

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get a pointer to the parent
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
			//get a pointer to the link for tha parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) * scaleFactor;
			//work out the local position of the node
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(PxVec3(childHalfLength, 0, 0));
			PxVec3 parentRelative = -currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(PxVec3(parentHalfLength, 0, 0));
			currentNodePtr->scaledGlobalPos = parentNode->scaledGlobalPos - (parentRelative + currentRelative);
		}

		//transformation for the link
		PxTransform linkTransform = PxTransform(currentNodePtr->scaledGlobalPos, currentNodePtr->globalRotation);
		//create link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);
		//add the pointer to the link into ragdoll data
		currentNodePtr->linkPtr = link;
		float jointSpace = 0.01f;
		float capsuleHalfLength = (halfLength > jointSpace ? halfLength - jointSpace : 0) + 0.01f;
		PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		link->createShape(capsule, *ragdollMaterial);
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f);

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get a pointer to the joint from the link
			PxArticulationJoint* joint = link->getInboundJoint();
			//get relative rotation of the link
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			//set the parent constraint frame
			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr->parentLinkPos * parentHalfLength, 0, 0), frameRotation);
			//set the child constraint frame
			PxTransform thisConstraintFrame = PxTransform(PxVec3(currentNodePtr->childLinkPos * childHalfLength, 0, 0));
			//set up poses for the joints
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//set up some constraints
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}

		currentNode++;
	}
	return articulation;
}