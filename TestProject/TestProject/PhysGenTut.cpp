#include "PhysGenTut.h"
#include "Gizmos.h"
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

PhysGenTut::PhysGenTut()
{

}

void PhysGenTut::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	srand(time(NULL));

	SetupTut1();

	//AntTweakBar
	m_bar = TwNewBar("my bar");

}

void PhysGenTut::Destroy()
{

}

void PhysGenTut::Update(float dt)
{
	m_physicsScene->Update();
	m_physicsScene->AddGizmos();

	camera.Update(dt);
}

void PhysGenTut::Draw()
{
	m_physicsScene->Draw(camera.GetProjectionView());
}


void PhysGenTut::SetupTut1()
{
	m_physicsScene = new PhysScene();
	m_physicsScene->gravity = glm::vec3(0, -10, 0);
	m_physicsScene->timeStep = .001f;
	//add 4 balls
	Sphere* newBall;
	newBall = new Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	m_physicsScene->AddActor(newBall);
}