#include "PhysXTut.h"
//#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <cstdlib>

#include <stb_image.h>

#include "myAllocator.h"

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
	cameraSpeed = camera.GetSpeed();
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

	//m_renderer.LoadFBX("../data/fbx/characters/Enemytank/EnemyTank.fbx");
	m_renderer.LoadTexture("m_texture", "../data/fbx/characters/Enemytank/EnemyTank_D.tga", GL_RGBA);
	m_renderer.SetModelTexture(0, "m_texture");

	//m_renderer.LoadFBX("../data/fbx/characters/enemynormal/EnemyNormal.fbx");
	m_renderer.LoadTexture("m_texture", "../data/fbx/characters/enemynormal/EnemyNormal1_D.tga", GL_RGB);
	m_renderer.SetModelTexture(1, "m_texture");

	for (int i = 2; i < 10; i++)
	{
		//m_renderer.LoadFBX("../data/fbx/trees/rock.fbx");
		m_renderer.LoadTexture("m_texture", "../data/fbx/trees/rockDiffuse.tga", GL_RGB);
		m_renderer.SetModelTexture(i, "m_texture");
	}

	m_renderer.CreateFB();
	m_renderer.CreateViewPlane();

	m_renderer.CreateSB();

	m_renderer.CreateDiamondSquare(65);
	m_renderer.CreateTerrainPlane(65, 65);

	m_renderer.CreateWaterPlane(65);

	int rand1 = rand() % 65;
	int rand2 = rand() % 65;

	mat4 pos = glm::translate(m_renderer.GetTerrainPos(rand1, rand2));
	mat4 scale = glm::scale(vec3(0.1f, 0.1f, 0.1f));

	m_renderer.FillModel(0, pos, mat4{ 1.0f }, scale);

	vec2 texcoord = m_renderer.GetTerrainTexCoord(rand1, rand2);
	m_renderer.SetModelHeightTexCoord(0, texcoord);
	m_renderer.SetAnimateModel(0, true);

	m_renderer.FillModel(1, mat4{ 1.0f }, mat4{ 1.0f }, scale);

	texcoord = m_renderer.GetTerrainTexCoord(0, 0);
	m_renderer.SetModelHeightTexCoord(1, texcoord);
	m_renderer.SetAnimateModel(1, false);

	for (int i = 2; i < 10; i++)
	{
		rand1 = rand() % 65;
		rand2 = rand() % 65;

		pos = glm::translate(m_renderer.GetTerrainPos(rand1, rand2));
		scale = glm::scale(vec3(0.5f, 0.5f, 0.5f));

		m_renderer.FillModel(i, pos, mat4{ 1.0f }, scale);

		texcoord = m_renderer.GetTerrainTexCoord(rand1, rand2);
		m_renderer.SetModelHeightTexCoord(i, texcoord);
		m_renderer.SetAnimateModel(i, false);
	}

}

void PhysXTut::Destroy()
{
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();

	m_renderer.Destroy();
}

void PhysXTut::Update(float dt)
{
	UpdatePhysX(dt);

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

	m_renderer.SetModelPos(1, modelPos);

	vec2 texcoord = vec2((modelPos.x + (64 * 50)) / (64 * 100), (modelPos.z + (64 * 50)) / (64 * 100));
	m_renderer.SetModelHeightTexCoord(1, texcoord);

	m_renderer.SetModelScale(1, modelScale);
	m_renderer.SetAnimateFBX(animate);

	m_renderer.Update(m_timer, dt, &camera.GetWorldTransform());
	camera.SetSpeed(cameraSpeed);
	camera.Update(dt);
}

void PhysXTut::Draw()
{
	m_renderer.Draw(&light, &lightColour, &mat4(), &camera.GetProjectionView(), &camera.GetPosition(), &specPow, &height, &waterHeight, &currentTime);
}

void PhysXTut::SetUpPhysX()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, 
		PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
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

	//add a box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(10, 10, 10));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
}