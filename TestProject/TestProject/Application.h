#pragma once
#include "AntTweakBar.h"
#include "gl_core_4_4.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <include\GLFW\glfw3.h>

using glm::vec4;

class Application
{
public:
	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

	int Run();

protected:
	float previousDeltaTime;
	float currentTime;
	unsigned int m_texture, m_normal;

	GLFWwindow* window;

	TwBar* m_bar;

	vec4 m_clearColour;

private:

};