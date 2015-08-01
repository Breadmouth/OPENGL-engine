#include "PhysXTut.h"
//#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <cstdlib>

#include "Gizmos.h"

#include <stb_image.h>

#include "myAllocator.h"

#include "Ragdoll.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

using namespace physx;

PhysXTut::PhysXTut()
{
	light = vec3(0, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 1.0f;
	animate = true;
	generate = false;
	height = 30;

	modelScale = 0.1f;
	modelPos = vec3(0, 0, 0);

	m_anim.x = 0.0f;
	m_anim.y = 3.7f;

	m_timer = m_anim.x;
}

void PhysXTut::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	srand(time(NULL));

	//physX init
	SetUpPhysX();

	AddPlayerController();

	SetUpVisualDebugger();

	SetupTut1();

	//AntTweakBar
	m_bar = TwNewBar("my bar");

	TwAddVarRW(m_bar, "generate terrain", TW_TYPE_BOOLCPP, &generate, "group=generation");
	TwAddVarRW(m_bar, "height", TW_TYPE_FLOAT, &height, "group=generation");
	TwAddVarRW(m_bar, "water height", TW_TYPE_FLOAT, &waterHeight, "group=generation");

	TwAddVarRW(m_bar, "model2 pos", TW_TYPE_DIR3F, &modelPos, "group=models");
	TwAddVarRW(m_bar, "model2 scale", TW_TYPE_FLOAT, &modelScale, "group=models");

	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "light dir", TW_TYPE_DIR3F, &light, "group=light");
	TwAddVarRW(m_bar, "light colour", TW_TYPE_COLOR3F, &lightColour, "group=light");
	TwAddVarRW(m_bar, "light spec", TW_TYPE_FLOAT, &specPow, "group=light");
	TwAddVarRW(m_bar, "camera speed", TW_TYPE_FLOAT, &cameraSpeed, "group=camera");
	TwAddVarRW(m_bar, "animate", TW_TYPE_BOOLCPP, &animate, "group=animation");

	//load shaders
	m_renderer.LoadShader("m_terrainGenProgram", "../data/shaders/terrainGen.glvs", "../data/shaders/terrainGen.glfs");
	m_renderer.LoadShader("m_terrainGenShadowProgram", "../data/shaders/terrainGenShadow.glvs", "../data/shaders/terrainGenShadow.glfs");
	m_renderer.LoadShader("m_waterProgram", "../data/shaders/water.glvs", "../data/shaders/water.glfs");
	m_renderer.LoadShader("m_postProcessProgram", "../data/shaders/tut10.glvs", "../data/shaders/tut10.glfs");
	m_renderer.LoadShader("m_programID", "../data/shaders/modelShader.glvs", "../data/shaders/modelShader.glfs");

	m_renderer.LoadShader("m_shadowProgram", "../data/shaders/modelShaderShadow.glvs", "../data/shaders/modelShaderShadow.glfs");
	m_renderer.LoadShader("m_shadowProgramAnim", "../data/shaders/modelShaderShadowAnim.glvs", "../data/shaders/modelShaderShadowAnim.glfs");
	m_renderer.LoadShader("m_shadowGenProgram", "../data/shaders/shadowMap.glvs", "../data/shaders/shadowMap.glfs");
	m_renderer.LoadShader("m_shadowGenProgramAnim", "../data/shaders/shadowMapAnim.glvs", "../data/shaders/shadowMapAnim.glfs");

	//load textures
	m_renderer.LoadTexture("m_grass_texture", "../data/grass.jpg", GL_RGBA);
	m_renderer.LoadTexture("m_rock_texture", "../data/rock.jpg", GL_RGB);
	m_renderer.LoadTexture("m_snow_texture", "../data/snow.jpg", GL_RGB);
	m_renderer.LoadTexture("m_water_texture", "../data/water.jpg", GL_RGB);
	m_renderer.LoadTexture("m_sand_texture", "../data/sand.jpg", GL_RGB);

	m_renderer.LoadTexture("m_water_height_1", "../data/waterheight1.jpg", GL_RGB);
	m_renderer.LoadTexture("m_water_height_2", "../data/waterheight2.jpg", GL_RGB);

	//m_renderer.LoadFBX("../data/fbx/cube.fbx");
	//m_renderer.LoadTexture("m_texture", "../data/grass.jpg", GL_RGBA);
	//m_renderer.SetModelTexture(m_renderer.GetModelCount()-1, "m_texture");

	m_renderer.CreateFB();
	m_renderer.CreateViewPlane();

	m_renderer.CreateSB();

	//m_renderer.CreateDiamondSquare(65);
	//m_renderer.CreateTerrainPlane(65, 65);

	//m_renderer.CreateWaterPlane(65);

	//int rand1 = rand() % 65;
	//int rand2 = rand() % 65;
	//
	//mat4 pos = glm::translate(m_renderer.GetTerrainPos(rand1, rand2));
	//mat4 scale = glm::scale(vec3(100.f, 100.f, 100.f));
	//
	//m_renderer.FillModel(0, pos, mat4{ 1.0f }, scale);
	//
	//vec2 texcoord = m_renderer.GetTerrainTexCoord(rand1, rand2);
	//m_renderer.SetModelHeightTexCoord(0, texcoord);
	//m_renderer.SetAnimateModel(0, false);

}

void PhysXTut::Destroy()
{
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();

	m_renderer.Destroy();
}

void PhysXTut::onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eDELETED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eDELETED_SHAPE_OTHER))
			continue;

		if ((pairs[i].otherShape->getGeometryType() == physx::PxGeometryType::eSPHERE))
		{
			m_triggered = true;
		}
	}
};

void PhysXTut::Update(float dt)
{
	UpdatePlayer(dt);
	UpdatePhysX(dt);

	if (m_triggered)
	{
		PxTransform transform = PxTransform(15, 5, 6);
		PxSphereGeometry sphere(1);
		PxReal density = 10;
		PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere, *g_PhysicsMaterial, density);
		g_PhysicsScene->addActor(*dynamicActor);
		g_PhysXActors.push_back(dynamicActor);

		m_triggered = false;
	}

	mouseClickCooldown -= dt;

	if (generate)
	{
		m_renderer.CreateDiamondSquare(65);
		m_renderer.CreateTerrainPlane(65, 65);
		generate = false;
	}

	if (animate)
	{
		m_timer += dt;
		if (m_timer > m_anim.y)
			m_timer = m_anim.x;
	}

	FireBall();

	m_renderer.SetAnimateFBX(animate);

	m_renderer.Update(m_timer, dt, &camera.GetWorldTransform());

	//camera.SetSpeed(cameraSpeed);
	camera.Update(dt);
}

void PhysXTut::Draw()
{
	Gizmos::draw(camera.GetProjectionView());

	//m_renderer.Draw(&light, &lightColour, &mat4(), &camera.GetProjectionView(), &camera.GetPosition(), &specPow, &height, &waterHeight, &currentTime);
}

PxFilterFlags MyFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

void PhysXTut::SetUpPhysX()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, 
		PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = MyFilterShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
}

void PhysXTut::AddPlayerController()
{
	myHitReport = new MyControllerHitReport();
	gCharacterManager = PxCreateControllerManager(*g_PhysicsScene);

	PxCapsuleControllerDesc desc;
	desc.height = 3.f;
	desc.radius = 2.f;
	desc.position.set(0, 0, 0);
	desc.material = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	desc.reportCallback = myHitReport;
	desc.density = 10;

	gPlayerController = gCharacterManager->createController(desc);

	gPlayerController->setPosition(PxExtendedVec3(0, 3, 0));

	m_characterYVelocity = 0.0f;
	m_characterRotation = 0.0f;
	m_playerGravity = -0.5f;
	myHitReport->clearPlayerContactNormal();

	//addToActorList(gPlayerController->getActor());
	g_PhysXActors.push_back(gPlayerController->getActor());

	camera.AttachToPlayerController(gPlayerController, &m_characterRotation);

}


void PhysXTut::UpdatePlayer(float dt)
{
	bool onGround;
	float movementSpeed = 10.0f;
	float rotationSpeed = 1.0f;

	m_yAccel -= movementSpeed * dt;

	if (myHitReport->getPlayerContactNormal().y > 0.3f)
	{
		m_characterYVelocity = -0.1f;
		onGround = true;
		m_yAccel = 0.f;
	}
	else
	{
		m_characterYVelocity += m_playerGravity * dt;
		onGround = false;
	}
	myHitReport->clearPlayerContactNormal();
	const PxVec3 up(0, 1, 0);

	PxVec3 velocity(0, m_characterYVelocity, 0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		velocity.x -= movementSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		velocity.x += movementSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_characterRotation += rotationSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_characterRotation -= rotationSpeed * dt;
	}
	//add code for jumping
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround)
	{
		m_yAccel = 30.0f;
	}

	velocity.y += m_yAccel * dt;

	float minDist = 0.001f;
	PxControllerFilters filter;
	PxQuat rotation(m_characterRotation, PxVec3(0, 1, 0));
	//move the controller
	gPlayerController->move(rotation.rotate(velocity), minDist, dt, filter);
}

void PhysXTut::UpdatePhysX(float dt)
{
	if (dt <= 0)
	{
		return;
	}
	g_PhysicsScene->simulate(dt);
	while (g_PhysicsScene->fetchResults() == false)
	{

	}

	for (auto actor : g_PhysXActors)
	{
		{
			PxU32 nShapes = actor->getNbShapes();
			PxShape** shapes = new PxShape*[nShapes];
			actor->getShapes(shapes, nShapes);

			while (nShapes--)
			{
				AddWidget(shapes[nShapes], actor);
			}
			delete[] shapes;
		}
	}

	for (auto art : g_PhysXActorRagdolls)
	{
		PxU32 nLinks = art->getNbLinks();
		PxArticulationLink** links = new PxArticulationLink*[nLinks];
		art->getLinks(links, nLinks);

		while (nLinks--)
		{
			PxArticulationLink* link = links[nLinks];
			PxU32 nShapes = link->getNbShapes();
			PxShape** shapes = new PxShape*[nShapes];
			link->getShapes(shapes, nShapes);
			while (nShapes--)
			{
				AddWidget(shapes[nShapes], link);
			}
		}
		delete[] links;
	}

	if (m_particleEmitter)
	{
		m_particleEmitter->update(dt);
		m_particleEmitter->renderParticles();
	}
}

void PhysXTut::AddWidget(PxShape* shape, PxRigidActor* actor)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
	case PxGeometryType::eBOX:
		AddBox(shape, actor);
		break;

	case PxGeometryType::ePLANE:
		AddPlane(shape, actor);
		break;

	case PxGeometryType::eSPHERE:
		AddSphere(shape, actor);
		break;

	case PxGeometryType::eCAPSULE:
		AddCapsule(shape, actor);
		break;

	default:
		break;
	}

}

void PhysXTut::AddBox(PxShape* pShape, PxRigidActor* actor)
{
	//get geometry for this collision volume
	PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if (status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}

	//get transform for the collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get position from transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width, height, length);
	glm::vec4 colour = glm::vec4(1, 0, 0, 1);

	Gizmos::addAABBFilled(position, extents, colour, &M);
	
	//m_renderer.LoadFBX("../data/fbx/cube.fbx");
	//m_renderer.LoadTexture("m_texture", "../data/grass.jpg", GL_RGBA);
	//m_renderer.SetModelTexture(m_renderer.GetModelCount() - 1, "m_texture");
	//
	//mat4 pos = glm::translate(position);
	//mat4 scale = glm::scale(vec3(10.f * width, 10.f * height, 10.f * length));
	//
	//m_renderer.FillModel(0, pos, mat4{ 1.0f }, scale);
	//
	//m_renderer.SetAnimateModel(0, false);
}

void PhysXTut::AddPlane(PxShape* pShape, PxRigidActor* actor)
{
	//get geometry for this collision volume
	PxPlaneGeometry geometry;
	bool status = pShape->getPlaneGeometry(geometry);
	if (status)
	{
		//
	}

	//get transform for the collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get position from transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(0.1f, 100.0f, 100.0f);
	glm::vec4 colour = glm::vec4(0, 0, 1, 1);

	Gizmos::addAABBFilled(position, extents, colour, &M);
}

void PhysXTut::AddSphere(PxShape* pShape, PxRigidActor* actor)
{
	//get geometry for this collision volume
	PxSphereGeometry geometry;
	bool status = pShape->getSphereGeometry(geometry);
	float radius = 1;
	if (status)
	{
		//
		radius = geometry.radius;
	}

	//get transform for the collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get position from transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec4 colour = glm::vec4(0, 1, 0, 1);

	Gizmos::addSphere(position, radius, 10, 10, colour, &M);
}

void PhysXTut::AddCapsule(PxShape* pShape, PxRigidActor* actor)
{
	//get geometry for this collision volume
	PxCapsuleGeometry geometry;
	bool status = pShape->getCapsuleGeometry(geometry);
	float radius = 1, height = 1;
	if (status)
	{
		//
		height = geometry.halfHeight;
		radius = geometry.radius;
	}

	//get transform for the collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor));
	glm::mat4* M = (glm::mat4*)(&m);
	glm::vec3 position;
	//get position from transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;

	glm::vec4 axis(height, 0.f, 0.f, 0.f);
	axis = *M * axis;

	glm::vec4 colour = glm::vec4(0.5f, 0.5f, 0.2f, 1);

	Gizmos::addSphere(position + axis.xyz(), radius, 10, 10, colour);
	Gizmos::addSphere(position - axis.xyz(), radius, 10, 10, colour);

	glm::mat4 m2 = glm::rotate(*M, 11 / 7.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	Gizmos::addCylinderFilled(position, radius, height, 10, colour, &m2);
}

void PhysXTut::SetUpVisualDebugger()
{
	//check if PVDconnection manager is avaliable on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
	{
		return;
	}
	//setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	//ip of the PC which is running pvd
	int port = 5425;
	//TCP port to connect to
	unsigned int timeout = 100;
	//timeout in milliseconds to wait for PVD to respond
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	//and now try to connect PxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(
		g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}

void PhysXTut::SetupTut1()
{

	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the PhysX scene
	g_PhysicsScene->addActor(*plane);
	g_PhysXActors.push_back(plane);

	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(15, 2, 15));
	PxRigidStatic* staticActor = PxCreateStatic(*g_Physics, transform, box, *g_PhysicsMaterial);

	g_PhysicsScene->addActor(*staticActor);
	g_PhysXActors.push_back(staticActor);

	PxTransform newtransform(PxVec3(15, 5, 0));
	triggerShape = PxCreateDynamic(*g_Physics, newtransform, PxBoxGeometry(1, 1, 1), *g_PhysicsMaterial, density);
	triggerShape->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	PxShape** pShapes = new PxShape*[triggerShape->getNbShapes()];
	triggerShape->getShapes(pShapes, triggerShape->getNbShapes());
	pShapes[0]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShapes[0]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	g_PhysicsScene->addActor(*triggerShape);
	g_PhysXActors.push_back(triggerShape);

	transform = PxTransform(-6, 40, -9);
	PxSphereGeometry sphere(2);
	PxRigidDynamic* dynamicActor2 = PxCreateDynamic(*g_Physics, transform, sphere, *g_PhysicsMaterial, density);
	g_PhysicsScene->addActor(*dynamicActor2);
	g_PhysXActors.push_back(dynamicActor2);

	transform = PxTransform(0, 20, 20);
	PxCapsuleGeometry capsule(2, 2);
	PxRigidDynamic* dynamicActor3 = PxCreateDynamic(*g_Physics, transform, capsule, *g_PhysicsMaterial, density);
	g_PhysicsScene->addActor(*dynamicActor3);
	g_PhysXActors.push_back(dynamicActor3);

	PxArticulation* ragdollArt;
	ragdollArt = Ragdoll::MakeRagdoll(g_Physics, ragdollData, PxTransform(PxVec3(0, 20, -20)), 0.3f, g_PhysicsMaterial);
	g_PhysicsScene->addArticulation(*ragdollArt);
	g_PhysXActorRagdolls.push_back(ragdollArt);

	PxBoxGeometry side1(4.5, 1, .5);
	PxBoxGeometry side2(.5, 1, 4.5);

	transform = PxTransform(PxVec3(-12.0f, 0.5, 4.0f));
	PxRigidStatic* wall = PxCreateStatic(*g_Physics, transform, side1, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*wall);
	g_PhysXActors.push_back(wall);

	transform = PxTransform(PxVec3(-12.0f, 0.5, -4.0f));
	wall = PxCreateStatic(*g_Physics, transform, side1, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*wall);
	g_PhysXActors.push_back(wall);

	transform = PxTransform(PxVec3(-16.0f, 0.5, 0));
	wall = PxCreateStatic(*g_Physics, transform, side2, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*wall);
	g_PhysXActors.push_back(wall);

	transform = PxTransform(PxVec3(-8.0f, 0.5, 0));
	wall = PxCreateStatic(*g_Physics, transform, side2, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*wall);
	g_PhysXActors.push_back(wall);
	
	PxParticleFluid* pf;

	PxU32 maxParticles = 1500;
	bool perParticleRestOffset = false;
	pf = g_Physics->createParticleFluid(maxParticles, perParticleRestOffset);

	pf->setRestParticleDistance(.3f);
	pf->setDynamicFriction(0.1f);
	pf->setStaticFriction(0.1f);
	pf->setDamping(0.1f);
	pf->setParticleMass(.1f);
	pf->setRestitution(0);

	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(100);

	if (pf)
	{
		g_PhysicsScene->addActor(*pf);
		m_particleEmitter = new ParticleFluidEmitter(maxParticles, PxVec3(-12, 10, 0), pf, .05);
		m_particleEmitter->setStartVelocityRange(-100.f, -250.f, -100.f, 100.f, -250.0f, 100.f);
	}
}

void PhysXTut::FireBall()
{
	if (glfwGetMouseButton(window, 0) && mouseClickCooldown < 0.0f)
	{
		float density = 100;
		PxSphereGeometry sphere(0.5);

		glm::vec3 startPos = camera.GetWorldTransform()[3].xyz;
		glm::vec3 dir = camera.GetWorldTransform()[2].xyz;
		physx::PxVec3 velocity = physx::PxVec3(dir.x, dir.y, dir.z) * -50;

		PxTransform transform = PxTransform(startPos.x, startPos.y, startPos.z);

		PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere, *g_PhysicsMaterial, density);
		
		dynamicActor->setLinearVelocity(velocity, true);

		g_PhysicsScene->addActor(*dynamicActor);
		g_PhysXActors.push_back(dynamicActor);

		mouseClickCooldown = 0.2f;
	}
}

//-----------------MycontrollerHitReport
void MyControllerHitReport::onShapeHit(const PxControllerShapeHit &hit)
{
	PxRigidActor* actor = hit.shape->getActor();

	_playerContactNormal = hit.worldNormal;

	PxRigidDynamic* myActor = actor->is<PxRigidDynamic>();
	if (myActor)
	{
		//apply force to things we hit here
	}
}