#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

#include <PxSimulationEventCallback.h>
#include <PxFiltering.h>

class TriggerCallback : public physx::PxSimulationEventCallback
{
private:
	TriggerCallback();
	~TriggerCallback();

	void TriggerCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];

			m_triggered = true;
		}

	};

	void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count){};
	void onWake(PxActor **actors, PxU32 count){};
	void onSleep(PxActor **actors, PxU32 count){};
	void onTrigger(PxTriggerPair *pairs, PxU32 count){};

	bool m_triggered;

public:

	bool GetTriggered(){ return m_triggered; };

};