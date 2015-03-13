#include "Tutorial9.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial9::Tutorial9()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;

	m_timer = 0.0f;

}

void Tutorial9::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	//AntTweakBar
	m_bar = TwNewBar("my bar");
	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "camera speed", TW_TYPE_FLOAT, &cameraSpeed, "group=camera");

	m_emitter = new GPUParticleEmitter();
	m_emitter->Init(100000, 0.5f, 5.0f, 5, 20, 1, 0, glm::vec4(0.25f, 0, 0.25f, 1), glm::vec4(0, 0.6f, 0.6f, 1));

}

void Tutorial9::Destroy()
{
	m_renderer.Destroy();
}

void Tutorial9::Update(float dt)
{
	if (animate)
		m_timer += dt;

	camera.SetSpeed(cameraSpeed);

	camera.Update(dt);
}

void Tutorial9::Draw()
{
	glUseProgram(m_programID);
	int projection_view_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionView()));

	m_emitter->Draw((float)glfwGetTime(), camera.GetWorldTransform(), camera.GetProjectionView());

	Gizmos::draw(camera.GetProjectionView());
}