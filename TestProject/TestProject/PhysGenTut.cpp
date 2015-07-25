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

	m_physicsScene->Update();
	m_physicsScene->AddGizmos();

	Gizmos::addAABBFilled(glm::vec3(0, 0, -2), glm::vec3(14, 8, 1), glm::vec4(0, 0.3, 0.8, 1));
	Gizmos::addAABBFilled(glm::vec3(0, 0, -1.5f), glm::vec3(12, 6, 1), glm::vec4(0, 0.7, 0, 1));

	camera.Update(dt);
}

void PhysGenTut::Draw()
{
	m_physicsScene->Draw(camera.GetProjectionView());
}


void PhysGenTut::SetupTut1()
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

	Box* boxxy;
	Box* boxxo;

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

	boxxy = new Box(glm::vec3(1.f, -3.f, 0), glm::vec3(0, 60.f, 0), glm::vec3(0, 0, 0), 1, 0.6f, 1, 1, 1, glm::vec4(1, 0, 1, 1), false);
	boxxo = new Box(glm::vec3(1.f, 3.f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1, 0.6f, 1, 1, 1, glm::vec4(1, 0, 1, 1), false);

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

	m_physicsScene->AddActor(boxxy);
	m_physicsScene->AddActor(boxxo);

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

}

void PhysGenTut::GetMouseInput()
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

			glm::vec3 shit = finalVal - m_whiteBall->GetPosition();

			m_whiteBall->ApplyForce(-shit * 50);

			lMouseButtonPressed = false;
		}
	}
}