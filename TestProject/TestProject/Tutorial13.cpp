#include "Tutorial13.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial13::Tutorial13()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;
	generate = false;

	m_timer = 0.0f;
}

void Tutorial13::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	//AntTweakBar
	m_bar = TwNewBar("my bar");

	TwAddVarRW(m_bar, "generate terrain", TW_TYPE_BOOLCPP, &generate, "group=generation");

	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "light dir", TW_TYPE_DIR3F, &light, "group=light");
	TwAddVarRW(m_bar, "light colour", TW_TYPE_COLOR3F, &lightColour, "group=light");
	TwAddVarRW(m_bar, "light spec", TW_TYPE_FLOAT, &specPow, "group=light");
	TwAddVarRW(m_bar, "camera speed", TW_TYPE_FLOAT, &cameraSpeed, "group=camera");
	TwAddVarRW(m_bar, "animate", TW_TYPE_BOOLCPP, &animate, "group=animation");

	m_renderer.LoadShader("m_perlinProgram", "../data/shaders/perlin.glvs", "../data/shaders/perlin.glfs");
	m_renderer.LoadShader("m_waterProgram", "../data/shaders/water.glvs", "../data/shaders/water.glfs");

	m_renderer.LoadTexture("m_snow_texture", "../data/snow.jpg", GL_RGB);
	m_renderer.LoadTexture("m_grass_texture", "../data/grass.jpg", GL_RGB);
	m_renderer.LoadTexture("m_rock_texture", "../data/water.jpg", GL_RGB);

	m_renderer.LoadShader("m_postProcessProgram", "../data/shaders/tut10.glvs", "../data/shaders/tut10.glfs");
	m_renderer.CreateFB();
	m_renderer.CreateViewPlane();

	m_renderer.CreateTerrainPlane(65, 65);
	m_renderer.CreateDiamondSquare(65);

	m_renderer.CreateWaterPlane(65);
	//m_renderer.CreatePerlin(5);
}

void Tutorial13::Destroy()
{
	m_renderer.Destroy();
}

void Tutorial13::Update(float dt)
{
	if (generate)
	{
		m_renderer.CreateDiamondSquare(65);
		generate = false;
	}

	m_renderer.Update(dt, dt, &camera.GetWorldTransform());
	camera.SetSpeed(cameraSpeed);
	camera.Update(dt);
}

void Tutorial13::Draw()
{
	//m_renderer.DrawPlane(&camera.GetProjectionView());
	m_renderer.Draw(&light, &lightColour, &mat4(), &camera.GetProjectionView(), &camera.GetPosition(), &specPow);
}