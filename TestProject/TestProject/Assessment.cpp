#include "Assessment.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <cstdlib>

#include <stb_image.h>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Assessment::Assessment()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;
	generate = false;
	height = 50;

	modelScale = 0.1;
	modelPos = vec3(0, 0, 0);

	m_anim.x = 0.0f;
	m_anim.y = 3.7f;

	m_timer = m_anim.x;
}

void Assessment::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	srand(time(NULL));

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
	m_renderer.LoadTexture("m_water_texture", "../data/water.jpg", GL_RGB);

	m_renderer.LoadFBX("../data/fbx/characters/Enemytank/EnemyTank.fbx");
	m_renderer.LoadTexture("m_texture", "../data/fbx/characters/Enemytank/EnemyTank_D.tga", GL_RGBA);
	m_renderer.SetModelTexture(0, "m_texture");

	m_renderer.LoadFBX("../data/fbx/characters/enemynormal/EnemyNormal.fbx");
	m_renderer.LoadTexture("m_texture", "../data/fbx/characters/enemynormal/EnemyNormal1_D.tga", GL_RGB);
	m_renderer.SetModelTexture(1, "m_texture");
	
	m_renderer.CreateFB();
	m_renderer.CreateViewPlane();

	m_renderer.CreateSB();

	m_renderer.CreateTerrainPlane(65, 65);
	m_renderer.CreateDiamondSquare(65);

	m_renderer.CreateWaterPlane(65);

	int rand1 = rand() % 65;
	int rand2 = rand() % 65;

	mat4 pos = glm::translate(m_renderer.GetTerrainPos(rand1, rand2));
	mat4 scale = glm::scale(vec3(0.1f, 0.1f, 0.1f));

	m_renderer.FillModel(0, pos, mat4{ 1.0f }, scale);
	vec2 texcoord = m_renderer.GetTerrainTexCoord(rand1, rand2);
	m_renderer.SetModelHeightTexCoord(0, texcoord);

	m_renderer.FillModel(1, mat4{1.0f}, mat4{ 1.0f }, scale);
	m_renderer.SetModelHeightTexCoord(1, texcoord);

}

void Assessment::Destroy()
{
	m_renderer.Destroy();
}

void Assessment::Update(float dt)
{
	if (generate)
	{
		m_renderer.CreateDiamondSquare(65);
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

void Assessment::Draw()
{
	m_renderer.Draw(&light, &lightColour, &mat4(), &camera.GetProjectionView(), &camera.GetPosition(), &specPow, &height, &waterHeight);
}