#include "PhysXCamera.h"

PhysXCamera::PhysXCamera()
{
	viewTransform = glm::lookAt(vec3(10, 0, 0), vec3(0), vec3(0, 1, 0));
	worldTransform = glm::inverse(viewTransform);
	SetPerspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 10000.f);

	ignoreInput = false;
}

void PhysXCamera::AttachToPlayerController(PxController* player, float* rotation)
{
	m_player = player;
	m_rotation = rotation;
}

void PhysXCamera::Update(float dt)
{
	if (m_player != nullptr)
	{
		PxQuat yRot = PxQuat(*m_rotation, PxVec3(0, 1, 0));
		PxVec3 t = yRot.getBasisVector0();
		SetPosition(vec3((float)m_player->getPosition().x - (2 * t.x), (float)m_player->getPosition().y + 2, (float)m_player->getPosition().z - (2 * t.z)));

		vec3 lookat = (vec3((float)m_player->getPosition().x - (2 * t.x), (float)m_player->getPosition().y + 2, (float)m_player->getPosition().z - (2 * t.z))) - vec3((float)t.x, (float)t.y, (float)t.z);

		SetLookAt(worldTransform[3].xyz(), lookat, vec3(0, 1, 0));
		float thing = 0;
	}

	//GetMouseInput(dt);

	UpdateProjectionViewTransform();
}

void PhysXCamera::GetMouseInput(float dt)
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

void PhysXCamera::CalculateRotation(float dt, double xOffset, double yOffset)
{
	glm::mat3 xRot = glm::mat3(glm::rotate((float)(-xOffset * dt), glm::vec3(0, 1, 0)));
	glm::mat3 yRot = glm::mat3(glm::rotate((float)(-yOffset * dt), glm::vec3(1, 0, 0)));

	if (glm::length(glm::vec2(xOffset, yOffset)) > 0.00f)
		SetTransform(GetWorldTransform() * glm::mat4(xRot * yRot));

	glm::mat4 oldTrans = GetWorldTransform();

	glm::vec3 m_sideVector = glm::cross(glm::vec3(0, 1, 0), glm::vec3(oldTrans[2]));
	glm::vec3 m_upVector = glm::cross(m_sideVector, glm::vec3(oldTrans[2]));

	m_sideVector = glm::normalize(m_sideVector);
	m_upVector = glm::normalize(m_upVector);

	oldTrans[0] = glm::vec4(m_sideVector, 0);
	oldTrans[1] = glm::vec4(-m_upVector, 0);
	oldTrans[2] = glm::normalize(oldTrans[2]);

	viewTransform = glm::inverse(oldTrans);
	SetTransform(oldTrans);
}