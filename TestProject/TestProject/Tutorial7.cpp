#include "Tutorial7.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial7::Tutorial7()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;

	m_timer = 0.0f;
}

void Tutorial7::Create()
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

	m_renderer.LoadShader("m_programID", "../data/shaders/fbxShader.glvs", "../data/shaders/fbxShader.glfs");

	m_renderer.LoadFBX("../data/fbx/characters/Enemytank/EnemyTank.fbx");

	m_renderer.LoadTexture("m_texture", "../data/fbx/characters/Enemytank/EnemyTank_D.tga", GL_RGBA);
	m_renderer.LoadNormal("../data/fbx/characters/Enemytank/EnemyTank_N.tga");

}

void Tutorial7::Destroy()
{
	m_renderer.Destroy();
}

void Tutorial7::Update(float dt)
{
	if (animate)
		m_timer += dt;

	m_renderer.UpdateFBX(m_timer);

	camera.SetSpeed(cameraSpeed);
	m_renderer.SetAnimateFBX(animate);

	camera.Update(dt);
}

void Tutorial7::Draw()
{
	//m_renderer.Draw(&camera.GetProjectionView(), &light, &camera.GetPosition(), &lightColour, &specPow);

	m_renderer.DrawFBX(&camera.GetProjectionView(), &light, &camera.GetPosition(), &lightColour, &specPow);

	Gizmos::draw(camera.GetProjectionView());

}