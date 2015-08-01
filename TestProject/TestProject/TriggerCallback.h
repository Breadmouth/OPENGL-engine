#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

#include <PxSimulationEventCallback.h>
#include <PxFiltering.h>

class TriggerCallback : public physx::PxSimulationEventCallback
{
public:
	TriggerCallback(){};
	~TriggerCallback(){};

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs){};

	void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count);

	void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count){};
	void onWake(physx::PxActor **actors, physx::PxU32 count){};
	void onSleep(physx::PxActor **actors, physx::PxU32 count){};

	bool m_triggered;

	bool GetTriggered(){ return m_triggered; };
	void SetTriggered(bool triggered){ m_triggered = triggered; };

};