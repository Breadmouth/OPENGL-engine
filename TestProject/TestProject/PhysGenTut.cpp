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
	m_window = glfwGetCurrentContext();

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
	GetMouseInput();
	CreateCube();
	CreateSphere();

	m_physicsScene->Update();
	m_physicsScene->AddGizmos();

	camera.Update(dt);

	m_createCooldown -= dt;
}

void PhysGenTut::Draw()
{
	m_physicsScene->Draw(camera.GetProjectionView());
}


void PhysGenTut::SetupTut1()
{
	m_physicsScene = new PhysScene();
	m_physicsScene->gravity = glm::vec3(0, -5, 0);
	m_physicsScene->timeStep = .001f;

	Sphere* ball1;
	Sphere* ball2;
	Sphere* ball3;
	Sphere* ball4;

	Sphere* staticBall;
	Sphere* bounceBall;

	SpringJoint* spring;

	Box* box1;
	Box* box2;
	Box* box3;

	Plane* plane1;
	Plane* plane2;
	Plane* plane3;
	Plane* plane4;
	Plane* plane5;
	Plane* plane6;

	ball1 = new Sphere(glm::vec3(1, -2, 6), glm::vec3(340, 250, -510), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball2 = new Sphere(glm::vec3(-4, 1, -2), glm::vec3(-110, -120, 160), glm::vec3(0, 0, 0), 2.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball3 = new Sphere(glm::vec3(-3, 3, 6), glm::vec3(-350, 240, -140), glm::vec3(0, 0, 0), 3.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball4 = new Sphere(glm::vec3(3, 4, -1), glm::vec3(120, -310, 420), glm::vec3(0, 0, 0), 4.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);

	staticBall = new Sphere(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.f, 0.8f, 0.5f, glm::vec4(1, 0.5, 0, 1), true);
	bounceBall = new Sphere(glm::vec3(1, -1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0.5, 0, 1), false);

	spring = new SpringJoint(staticBall, bounceBall, 10, 0.01f);

	box1 = new Box(glm::vec3(2, -5, 1), glm::vec3(-340, -460, 240), glm::vec3(0, 0, 0), 2, 0.6f, 1, 1, 1, glm::vec4(1, 0, 1, 1), false);
	box2 = new Box(glm::vec3(-3, 2, 3), glm::vec3(0, -130, 40), glm::vec3(0, 0, 0), 4, 0.6f, 1, 1, 1, glm::vec4(1, 0, 1, 1), false);
	box3 = new Box(glm::vec3(-4, 3, -1), glm::vec3(230, 350, -530), glm::vec3(0, 0, 0), 6, 0.6f, 1, 1, 1, glm::vec4(1, 0, 1, 1), false);

	//bottom
	plane1 = new Plane(glm::vec3(0, 1, 0), -6, glm::vec4(.2, .2, .2, 1));
	//left
	plane2 = new Plane(glm::vec3(1, 0, 0), -6, glm::vec4(.2, .2, .2, 1));
	//right
	plane3 = new Plane(glm::vec3(1, 0, 0), 6, glm::vec4(.2, .2, .2, 1));
	//top
	plane4 = new Plane(glm::vec3(0, 1, 0), 6, glm::vec4(.2, .2, .2, 1));
	//forward
	plane5 = new Plane(glm::vec3(0, 0, 1), 6, glm::vec4(.2, .2, .2, 1));
	//back
	plane6 = new Plane(glm::vec3(0, 0, 1), -6, glm::vec4(.2, .2, .2, 1));

	m_physicsScene->AddActor(ball1);
	m_physicsScene->AddActor(ball2);
	m_physicsScene->AddActor(ball3);
	m_physicsScene->AddActor(ball4);

	m_physicsScene->AddActor(staticBall);
	m_physicsScene->AddActor(bounceBall);

	m_physicsScene->AddActor(spring);

	m_physicsScene->AddActor(box1);
	m_physicsScene->AddActor(box2);
	m_physicsScene->AddActor(box3);

	m_physicsScene->AddActor(plane1);
	m_physicsScene->AddActor(plane2);
	m_physicsScene->AddActor(plane3);
	m_physicsScene->AddActor(plane4);
	m_physicsScene->AddActor(plane5);
	m_physicsScene->AddActor(plane6);

}

void PhysGenTut::CreatePool()
{
	m_physicsScene = new PhysScene();
	m_physicsScene->gravity = glm::vec3(0, 0, 0);
	m_physicsScene->timeStep = .001f;

	Sphere* ball1;
	Sphere* ball2;
	Sphere* ball3;
	Sphere* ball4;
	Sphere* ball5;
	Sphere* ball6;
	Sphere* ball7;
	Sphere* ball8;
	Sphere* ball9;
	Sphere* ball10;
	Sphere* ball11;
	Sphere* ball12;
	Sphere* ball13;
	Sphere* ball14;
	Sphere* blackBall;

	Box* leftWall;
	Box* rightWall;
	Box* topWallLeft;
	Box* bottomWallLeft;
	Box* topWallRight;
	Box* bottomWallRight;

	Plane* newPlane;
	Plane* newPlane2;
	Plane* newPlane3;
	Plane* newPlane4;
	Plane* floor;

	m_whiteBall = new Sphere(glm::vec3(0.5f * 0.75f * -tableWidth, 0, 0.f), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 1, 1, 1), false);
	//m_whiteBall = new Sphere(glm::vec3(-12.f, 3.f, 5.f), glm::vec3(0, 0, -50.f), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 1, 1, 1), false);

	ball1 = new Sphere(glm::vec3(tableWidth / 4, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball2 = new Sphere(glm::vec3(tableWidth / 4 + (1 * 0.708), 0.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball3 = new Sphere(glm::vec3(tableWidth / 4 + (1 * 0.708), -0.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	ball4 = new Sphere(glm::vec3(tableWidth / 4 + (2 * 0.708), 1.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	ball5 = new Sphere(glm::vec3(tableWidth / 4 + (2 * 0.708), -1.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball6 = new Sphere(glm::vec3(tableWidth / 4 + (3 * 0.708), 1.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	ball7 = new Sphere(glm::vec3(tableWidth / 4 + (3 * 0.708), 0.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball8 = new Sphere(glm::vec3(tableWidth / 4 + (3 * 0.708), -0.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	ball9 = new Sphere(glm::vec3(tableWidth / 4 + (3 * 0.708), -1.5f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball10 = new Sphere(glm::vec3(tableWidth / 4 + (4 * 0.708), 2.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	ball11 = new Sphere(glm::vec3(tableWidth / 4 + (4 * 0.708), 1.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball12 = new Sphere(glm::vec3(tableWidth / 4 + (4 * 0.708), 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	ball13 = new Sphere(glm::vec3(tableWidth / 4 + (4 * 0.708), -1.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(1, 0, 0, 1), false);
	ball14 = new Sphere(glm::vec3(tableWidth / 4 + (4 * 0.708), -2.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 1, 1), false);
	blackBall = new Sphere(glm::vec3(tableWidth / 4 + (2 * 0.708), 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1.f, 0.8f, 0.5f, glm::vec4(0, 0, 0, 1), false);

	rightWall = new Box(glm::vec3(12.f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.0f, 0.6f, 8.3f, 1, 1, glm::vec4(0, 0.5, 0, 1), true);
	leftWall = new Box(glm::vec3(-12.f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.0f, 0.6f, 8.3f, 1, 1, glm::vec4(0, 0.5, 0, 1), true);
	topWallLeft = new Box(glm::vec3(-5.56f, 5.56f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.0f, 0.6f, 1, 9.6f, 1, glm::vec4(0, 0.5, 0, 1), true);
	bottomWallLeft = new Box(glm::vec3(-5.56f, -5.56f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.0f, 0.6f, 1, 9.6f, 1, glm::vec4(0, 0.5, 0, 1), true);
	topWallRight = new Box(glm::vec3(5.56f, 5.56f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.0f, 0.6f, 1, 9.6f, 1, glm::vec4(0, 0.5, 0, 1), true);
	bottomWallRight = new Box(glm::vec3(5.56f, -5.56f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.0f, 0.6f, 1, 9.6f, 1, glm::vec4(0, 0.5, 0, 1), true);

	//bottom
	newPlane = new Plane(glm::vec3(0, 1, 0), -13, glm::vec4(.2, .2, .2, 1));
	//left
	newPlane2 = new Plane(glm::vec3(1, 0, 0), -13, glm::vec4(.2, .2, .2, 1));
	//right
	newPlane3 = new Plane(glm::vec3(1, 0, 0), 13, glm::vec4(.2, .2, .2, 1));
	//top
	newPlane4 = new Plane(glm::vec3(0, 1, 0), 13, glm::vec4(.2, .2, .2, 1));
	//floor
	floor = new Plane(glm::vec3(0, 0, 1), -1, glm::vec4(.2, .2, .2, 1));


	m_physicsScene->AddActor(m_whiteBall);

	m_physicsScene->AddActor(ball1);
	m_physicsScene->AddActor(ball2);
	m_physicsScene->AddActor(ball3);
	m_physicsScene->AddActor(ball4);
	m_physicsScene->AddActor(ball5);
	m_physicsScene->AddActor(ball6);
	m_physicsScene->AddActor(ball7);
	m_physicsScene->AddActor(ball8);
	m_physicsScene->AddActor(ball9);
	m_physicsScene->AddActor(ball10);
	m_physicsScene->AddActor(ball11);
	m_physicsScene->AddActor(ball12);
	m_physicsScene->AddActor(ball13);
	m_physicsScene->AddActor(ball14);
	m_physicsScene->AddActor(blackBall);

	m_physicsScene->AddActor(leftWall);
	m_physicsScene->AddActor(rightWall);
	m_physicsScene->AddActor(topWallLeft);
	m_physicsScene->AddActor(bottomWallLeft);
	m_physicsScene->AddActor(topWallRight);
	m_physicsScene->AddActor(bottomWallRight);

	m_physicsScene->AddActor(newPlane);
	m_physicsScene->AddActor(newPlane2);
	m_physicsScene->AddActor(newPlane3);
	m_physicsScene->AddActor(newPlane4);
	m_physicsScene->AddActor(floor);

	m_useMouseInput = true;
}

void PhysGenTut::CreateCube()
{
	if (glfwGetKey(m_window, GLFW_KEY_B) == GLFW_PRESS && m_createCooldown <= 0.f)
	{
		Box* newBox;
		newBox = new Box(glm::vec3(0, 0, 0), glm::vec3(rand() % 100, rand() % 100, rand() % 100), glm::vec3(0, 0, 0), 1, 0.6f, 1, 1, 1, glm::vec4(0, 1, 0, 1), false);
		m_physicsScene->AddActor(newBox);

		m_createCooldown = 0.5f;
	}
}

void PhysGenTut::CreateSphere()
{
	if (glfwGetKey(m_window, GLFW_KEY_N) == GLFW_PRESS && m_createCooldown <= 0.f)
	{
		Sphere* newSphere;
		newSphere = new Sphere(glm::vec3(0, 0, 0), glm::vec3(rand() % 100, rand() % 100, rand() % 100), glm::vec3(0, 0, 0), 1, 0.8f, 0.5, glm::vec4(0, 0, 1, 1), false);
		m_physicsScene->AddActor(newSphere);

		m_createCooldown = 0.5f;
	}
}

void PhysGenTut::GetMouseInput()
{
	if (m_useMouseInput)
	{
		if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		{
			lMouseButtonPressed = true;
		}
		else
		{
			if (lMouseButtonPressed)
			{
				double mouseX, mouseY;
				glfwGetCursorPos(m_window, &mouseX, &mouseY);

				int width, height;
				glfwGetFramebufferSize(m_window, &width, &height);

				glm::vec3 screenPos(mouseX / width * 2 - 1, (mouseY / height * 2 - 1) * -1, -1);

				screenPos.x /= camera.GetProjection()[0][0];
				screenPos.y /= camera.GetProjection()[1][1];

				glm::vec3 dir = glm::normalize(camera.GetWorldTransform() * glm::vec4(screenPos, 0)).xyz();

				float PoSubLoDotN = glm::dot((glm::vec3(0, 0, 1) * 0) - camera.GetWorldTransform()[3].xyz(), glm::vec3(0, 0, 1));
				float LDotN = glm::dot(dir, glm::vec3(0, 0, 1));

				float d = PoSubLoDotN / LDotN;

				glm::vec3 finalVal = camera.GetWorldTransform()[3].xyz() + dir * d;

				glm::vec3 realFinalVal = finalVal - m_whiteBall->GetPosition();

				m_whiteBall->ApplyForce(-realFinalVal * 50);

				lMouseButtonPressed = false;
			}
		}
	}
}