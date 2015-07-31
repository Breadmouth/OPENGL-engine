#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "PhysXCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"
#include "TriggerCallback.h"

//physX headers
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

#include <particles\PxParticleBase.h>
#include <particles\PxParticleSystem.h>
#include <particles\PxParticleFluid.h>
#include <particles\PxParticleCreationData.h>
#include <particles\PxParticleFlag.h>

#include <vector>

using glm::vec2;

using namespace physx;

class MyControllerHitReport : public PxUserControllerHitReport
{
public:
	virtual void onShapeHit(const PxControllerShapeHit & hit);

	virtual void onControllerHit(const PxControllersHit & hit){};
	virtual void onObstacleHit(const PxControllerObstacleHit &hit){};

	MyControllerHitReport() :PxUserControllerHitReport(){};
	PxVec3 getPlayerContactNormal(){ return _playerContactNormal; };
	void clearPlayerContactNormal(){ _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3 _playerContactNormal;
};

class PhysXTut : public Application
{
public:
	PhysXTut();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void SetUpPhysX();
	void UpdatePhysX(float dt);
	void SetUpVisualDebugger();

	void SetupTut1();
	void AddPlayerController();
	void UpdatePlayer(float dt);

	void AddWidget(PxShape* shape, PxRigidActor* actor);

	void AddBox(PxShape* pShape, PxRigidActor* actor);
	void AddPlane(PxShape* pShape, PxRigidActor* actor);
	void AddSphere(PxShape* pShape, PxRigidActor* actor);
	void AddCapsule(PxShape* pShape, PxRigidActor* actor);

	void FireBall();

protected:
	PhysXCamera camera;

	Renderer m_renderer;
	//GPUParticleEmitter* m_emitter;

	//physX variables
	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;

	MyControllerHitReport* myHitReport;
	PxControllerManager* gCharacterManager;
	PxController* gPlayerController;

	ParticleFluidEmitter* m_particleEmitter;

	TriggerCallback m_trigger;

	float m_characterYVelocity;
	float m_characterRotation;
	float m_playerGravity;
	float m_yAccel;

	std::vector<PxRigidActor*> g_PhysXActors;
	std::vector<PxArticulation*> g_PhysXActorRagdolls;

	float mouseClickCooldown = 0.2f;

	vec3 light;
	vec3 lightColour;
	float specPow;
	float cameraSpeed;
	bool animate;

	bool generate;
	float height;
	float waterHeight;
	vec3 modelPos;
	float modelScale;

	float m_timer;
	glm::vec2 m_anim;
};