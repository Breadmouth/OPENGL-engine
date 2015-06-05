#include "PhysScene.h"

PhysScene::PhysScene()
{
	
}

PhysScene::~PhysScene()
{

}

void PhysScene::Update()
{
	for each(Actor* actor in actors)
	{
		actor->Update(gravity, timeStep);
	}
}

void PhysScene::Draw(glm::mat4 projView)
{
	//Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / 1280, 100 / 720, -1.0f, 1.0f));
	Gizmos::draw(projView);
}

void PhysScene::AddGizmos()
{
	for each(Actor* actor in actors)
	{
		actor->MakeGizmo();
	}
}

void PhysScene::AddActor(Actor* actor)
{
	actors.push_back(actor);
}