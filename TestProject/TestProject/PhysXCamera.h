#pragma once

#include "Camera.h"
#include <gl_core_4_4.h>
#include <include\GLFW\glfw3.h>

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

using namespace physx;

class PhysXCamera : public Camera
{
private:
	vec3 up;
	GLFWwindow* window;

	PxController* m_player;
	float* m_rotation;

	bool rMouseButtonPressed;
	double cursorX, cursorY;

	void CalculateRotation(float dt, double xOffset, double yOffset);

public:
	PhysXCamera();

	virtual void Update(float dt);

	void GetMouseInput(float dt);

	void AttachToPlayerController(PxController* player, float* rotation);

	void SetInputWindow(GLFWwindow* Window) { window = Window; }
};