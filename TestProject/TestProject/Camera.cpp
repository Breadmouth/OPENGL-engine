#include "Camera.h"

void Camera::SetPerspective(float fov, float aspectR, float n, float f)
{
	projectionTransform= glm::perspective(fov, aspectR, n, f);
	perspectiveSet = true;

	UpdateProjectionViewTransform();
}

void Camera::SetLookAt(vec3 from, vec3 to, vec3 up)
{
	worldTransform = glm::inverse(glm::lookAt(from, to, up));
	UpdateProjectionViewTransform();
}

void Camera::SetPosition(vec3 pos)
{
	worldTransform[3] = glm::vec4(pos, 1);

	UpdateProjectionViewTransform();
}

void Camera::SetTransform(glm::mat4 transform)
{
	worldTransform = transform;
	UpdateProjectionViewTransform();
}

mat4 Camera::GetWorldTransform()
{
	return worldTransform;
}

mat4 Camera::GetView()
{
	return viewTransform;
}

mat4 Camera::GetProjection()
{
	return projectionTransform;
}

mat4 Camera::GetProjectionView()
{
	return projectionViewTransform;
}

void Camera::UpdateProjectionViewTransform()
{
	viewTransform = glm::inverse(worldTransform);
	projectionViewTransform = projectionTransform * viewTransform;
}

void Camera::SetIgnoreInput(bool ignore)
{
	ignoreInput = ignore;
}