#include "Tutorial6.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial6::Tutorial6()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
}

void Tutorial6::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	//AntTweakBar
	m_bar = TwNewBar("my bar");
	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "light dir", TW_TYPE_DIR3F, &light, "group=light");
	TwAddVarRW(m_bar, "light colour", TW_TYPE_COLOR3F, &lightColour, "group=light");
	TwAddVarRW(m_bar, "light spec", TW_TYPE_FLOAT, &specPow, "group=light");
	TwAddVarRW(m_bar, "camera speed", TW_TYPE_FLOAT, &cameraSpeed, "group=camera");

	m_renderer.LoadShader("m_programID", "../data/shaders/tinyShader.glvs", "../data/shaders/tinyShader.glfs");

	//m_renderer.LoadTexture("../objects/Raptor/Texture/raptor.jpg");
	m_renderer.LoadObject("../objects/Raptor/Raptor.obj");

}

void Tutorial6::Destroy()
{

}

void Tutorial6::Update(float dt)
{
	camera.SetSpeed(cameraSpeed);

	camera.Update(dt);
}

void Tutorial6::Draw()
{
//	m_renderer.DrawOBJ(&camera.GetProjectionView(), &light, &camera.GetPosition(), &lightColour, &specPow);

	Gizmos::draw(camera.GetProjectionView());

}