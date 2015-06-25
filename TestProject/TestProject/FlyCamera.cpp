#include "FlyCamera.h"

FlyCamera::FlyCamera()
{
	viewTransform = glm::lookAt(vec3(0,0,20), vec3(0), vec3(0,1,0));
	worldTransform = glm::inverse(viewTransform);
	SetPerspective(glm::pi<float>() * 0.25f, 16/9.f, 0.1f, 10000.f);

	ignoreInput = false;

	speed = 20.f;
}

void FlyCamera::Update(float dt)
{
	GetKeyboardInput(dt);
	GetMouseInput(dt);

	UpdateProjectionViewTransform();
}

void FlyCamera::GetKeyboardInput(float dt)
{
	if (!ignoreInput)
	{
		glm::vec3 vRight = worldTransform[0].xyz;
		glm::vec3 vUp = worldTransform[1].xyz;
		glm::vec3 vForward = worldTransform[2].xyz;

		glm::vec3 moveDir(0.0f);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			moveDir -= vForward;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			moveDir += vForward;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			moveDir -= vRight;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			moveDir += vRight;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
		}

		float fLength = glm::length(moveDir);
		if (fLength > 0.01f)
		{
			moveDir = ((float)dt * speed) * glm::normalize(moveDir);
			SetPosition(GetPosition() + moveDir);
		}
	}
}

void FlyCamera::GetMouseInput(float dt)
{
	if (!ignoreInput)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			if (rMouseButtonPressed == false)
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				cursorX = width / 2.0;
				cursorY = height / 2.0;

				glfwSetCursorPos(window, cursorX, cursorY);
				rMouseButtonPressed = true;
			}
			else
			{
				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

				double xOffset = mouseX - cursorX;
				double yOffset = mouseY - cursorY;

				cursorX = mouseX;
				cursorY = mouseY;

				CalculateRotation(dt, xOffset, yOffset);
			}
		}
		else
		{
			rMouseButtonPressed = false;
		}
	}
}

void FlyCamera::CalculateRotation(float dt, double xOffset,double yOffset)
{
	glm::mat3 xRot = glm::mat3(glm::rotate( (float)(-xOffset * dt), glm::vec3(0,1,0)));
	glm::mat3 yRot = glm::mat3(glm::rotate( (float)(-yOffset * dt), glm::vec3(1,0,0)));

	if(glm::length(glm::vec2(xOffset, yOffset)) > 0.00f)
		SetTransform( GetWorldTransform() * glm::mat4(xRot * yRot) );

	glm::mat4 oldTrans = GetWorldTransform();

	glm::vec3 m_sideVector = glm::cross( glm::vec3(0,1,0), glm::vec3(oldTrans[2]) );
	glm::vec3 m_upVector   = glm::cross(     m_sideVector, glm::vec3(oldTrans[2]) );

	m_sideVector = glm::normalize(m_sideVector);
	m_upVector   = glm::normalize(  m_upVector);

	oldTrans[0] = glm::vec4(m_sideVector, 0);
	oldTrans[1] = glm::vec4(  -m_upVector, 0);
	oldTrans[2] = glm::normalize(oldTrans[2]);

	viewTransform = glm::inverse(oldTrans);
	SetTransform(oldTrans);
}

void FlyCamera::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}

float FlyCamera::GetSpeed()
{
	return speed;
}