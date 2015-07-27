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

	void CheckForCollision();

	//collision check functions
	static bool PlaneToPlane(Actor* obj1, Actor* obj2);
	static bool PlaneToSphere(Actor* obj1, Actor* obj2);
	static bool PlaneToBox(Actor* obj1, Actor* obj2);
	static bool PlaneToJoint(Actor* obj1, Actor* obj2);
	static bool SphereToPlane(Actor* obj1, Actor* obj2);
	static bool SphereToSphere(Actor* obj1, Actor* obj2);
	static bool SphereToBox(Actor* obj1, Actor* obj2);
	static bool SphereToJoint(Actor* obj1, Actor* obj2);
	static bool BoxToPlane(Actor* obj1, Actor* obj2);
	static bool BoxToSphere(Actor* obj1, Actor* obj2);
	static bool BoxToBox(Actor* obj1, Actor* obj2);
	static bool BoxToJoint(Actor* obj1, Actor* obj2);
	static bool JointToPlane(Actor* obj1, Actor* obj2);
	static bool JointToSphere(Actor* obj1, Actor* obj2);
	static bool JointToBox(Actor* obj1, Actor* obj2);
	static bool JointToJoint(Actor* obj1, Actor* obj2);
};