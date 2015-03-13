#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
protected:
	mat4 worldTransform;
	mat4 viewTransform;
	mat4 projectionTransform;
	mat4 projectionViewTransform;

	bool perspectiveSet;
	bool ignoreInput;

public:
	virtual void Update(float dt) = 0;
	void SetPerspective(float fov, float aspectR, float n, float f);
	void SetLookAt(vec3 from, vec3 to, vec3 up);
	void SetPosition(vec3 pos);
	void SetTransform(glm::mat4 transform);

	void SetIgnoreInput(bool ignore);

	mat4 GetWorldTransform();
	mat4 GetView();
	mat4 GetProjection();
	mat4 GetProjectionView();

	glm::vec3 GetPosition() const { return worldTransform[3].xyz(); }
	
	void UpdateProjectionViewTransform();
};