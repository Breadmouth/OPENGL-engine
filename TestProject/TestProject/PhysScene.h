#pragma once

#include "gl_core_4_4.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <include\GLFW\glfw3.h>

#include <vector>
#include "Actors.h"

class PhysScene
{
public:
	glm::vec3 gravity;
	float timeStep;
	std::vector<Actor*> actors;

	PhysScene();
	~PhysScene();

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);
	void Update();
	void Draw(glm::mat4 projView);
	void DebugScene();
	void AddGizmos();

};