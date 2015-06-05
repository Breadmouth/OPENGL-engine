#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"

//physX headers
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
#include <vector>

using glm::vec2;

using namespace physx;

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

protected:
	FlyCamera camera;

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