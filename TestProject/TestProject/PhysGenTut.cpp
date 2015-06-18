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
	Sphere* newBall2;
	Box* newBox;
	Box* newBox2;
	Plane* newPlane;
	Plane* newPlane2;
	Plane* newPlane3;
	Plane* newPlane4;
	newBall = new Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	newBall2 = new Sphere(glm::vec2(4, 0), glm::vec2(-40, 0), 6.0f, 1, glm::vec4(0, 0, 1, 1));
	newBox = new Box(glm::vec2(2, 0), glm::vec2(-20, 0), 5.0f, 1, 1, glm::vec4(1, 0, 1, 1));
	newBox2 = new Box(glm::vec2(-2, 0), glm::vec2(20, 0), 5.0f, 1, 1, glm::vec4(1, 0, 1, 1));
	//bottom
	newPlane = new Plane(glm::vec2(0, 1), -10, glm::vec4(.2, .2, .2, 1));
	//left
	newPlane2 = new Plane(glm::vec2(1, 0), -10, glm::vec4(.2, .2, .2, 1));
	//right
	newPlane3 = new Plane(glm::vec2(1, 0), 10, glm::vec4(.2, .2, .2, 1));
	//top
	newPlane4 = new Plane(glm::vec2(0, 1), 10, glm::vec4(.2, .2, .2, 1));
	m_physicsScene->AddActor(newBall);
	m_physicsScene->AddActor(newBall2);
	m_physicsScene->AddActor(newBox);
	m_physicsScene->AddActor(newBox2);

	m_physicsScene->AddActor(newPlane);
	m_physicsScene->AddActor(newPlane2);
	m_physicsScene->AddActor(newPlane3);
	m_physicsScene->AddActor(newPlane4);

}