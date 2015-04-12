#include "Tutorial11.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial11::Tutorial11()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;

	m_timer = 0.0f;
}

void Tutorial11::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	//AntTweakBar
	m_bar = TwNewBar("my bar");
	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "light dir", TW_TYPE_DIR3F, &light, "group=light");
	TwAddVarRW(m_bar, "light colour", TW_TYPE_COLOR3F, &lightColour, "group=light");
	TwAddVarRW(m_bar, "light spec", TW_TYPE_FLOAT, &specPow, "group=light");
	TwAddVarRW(m_bar, "camera speed", TW_TYPE_FLOAT, &cameraSpeed, "group=camera");
	TwAddVarRW(m_bar, "animate", TW_TYPE_BOOLCPP, &animate, "group=animation");

	m_renderer.LoadShader("m_programID", "../data/shaders/modelShader.glvs", "../data/shaders/modelShader.glfs");
	m_renderer.LoadShader("m_shadowProgram", "../data/shaders/modelShaderShadow.glvs", "../data/shaders/modelShaderShadow.glfs");
	m_renderer.LoadShader("m_shadowProgramAnim", "../data/shaders/modelShaderShadowAnim.glvs", "../data/shaders/modelShaderShadowAnim.glfs");
	m_renderer.LoadShader("m_shadowGenProgram", "../data/shaders/shadowMap.glvs", "../data/shaders/shadowMap.glfs");
	m_renderer.LoadShader("m_shadowGenProgramAnim", "../data/shaders/shadowMapAnim.glvs", "../data/shaders/shadowMapAnim.glfs");

	m_renderer.LoadFBX("../data/fbx/characters/Enemytank/EnemyTank.fbx");
	m_renderer.LoadTexture("m_texture" ,"../data/fbx/characters/Enemytank/EnemyTank_D.tga", GL_RGBA);
	//m_renderer.LoadNormal("../data/fbx/characters/Enemytank/EnemyTank_N.tga");
	//m_renderer.LoadSpecular("../data/fbx/characters/Enemytank/EnemyTank_S.tga");

	m_renderer.CreateShadowPlane();

	m_renderer.LoadShader("m_postProcessProgram", "../data/shaders/tut10.glvs", "../data/shaders/tut10.glfs");
	m_renderer.CreateFB();
	m_renderer.CreateViewPlane();

	m_renderer.CreateSB();

}

void Tutorial11::Destroy()
{
	m_renderer.Destroy();
}

void Tutorial11::Update(float dt)
{
	if (animate)
		m_timer += dt;

	m_renderer.Update(m_timer, dt, &camera.GetWorldTransform());

	camera.SetSpeed(cameraSpeed);
	m_renderer.SetAnimateFBX(animate);

	camera.Update(dt);
}

void Tutorial11::Draw()
{
	float height = 50;
	float waterHeight = 0.0f;
	m_renderer.Draw(&light, &lightColour, &m_lightMatrix, &camera.GetProjectionView(), &camera.GetPosition(), &specPow, &height, &waterHeight);
}