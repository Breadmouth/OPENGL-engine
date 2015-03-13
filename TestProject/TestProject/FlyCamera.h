#pragma once

#include "Camera.h"
#include <gl_core_4_4.h>
#include <include\GLFW\glfw3.h>


class FlyCamera : public Camera
{
private:
	float speed;
	vec3 up;
	GLFWwindow* window;

	bool rMouseButtonPressed;
	double cursorX, cursorY;

	void CalculateRotation(float dt, double xOffset, double yOffset);

public:
	FlyCamera();

	virtual void Update(float dt);
	void SetSpeed(float newSpeed);
	float GetSpeed();

	void GetKeyboardInput(float dt);
	void GetMouseInput(float dt);

	void SetInputWindow(GLFWwindow* Window) { window = Window; }
};