#include <stdio.h>
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
	CheckForCollision();
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

typedef bool (*fn)(Actor*, Actor*);

static fn collisionFunctionArray[] =
{
	PhysScene::PlaneToPlane,
	PhysScene::PlaneToSphere,
	PhysScene::PlaneToBox,
	PhysScene::SphereToPlane,
	PhysScene::SphereToSphere,
	PhysScene::SphereToBox,
	PhysScene::BoxToPlane,
	PhysScene::BoxToSphere,
	PhysScene::BoxToBox,
};

void PhysScene::CheckForCollision()
{
	int actorCount = actors.size();
	//need to check for collisions for all objects except this one
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			Actor* object1 = actors[outer];
			Actor* object2 = actors[inner];
			int _shapeID1 = object1->GetShapeType();
			int _shapeID2 = object2->GetShapeType();
			//using function pointers
			int functionIndex = (_shapeID1 * NUMBERSHAPE) + _shapeID2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];
			if (collisionFunctionPtr != NULL)
			{
				collisionFunctionPtr(object1, object2);
			}
		}
	}
}

bool PhysScene::SphereToSphere(Actor* obj1, Actor* obj2)
{
	//try to cast to sphere and sphere
	Sphere* sphere1 = dynamic_cast<Sphere*>(obj1);
	Sphere* sphere2 = dynamic_cast<Sphere*>(obj2);
	//if successful then test collision
	if (sphere1 != NULL && sphere2 != NULL)
	{

		glm::vec2 delta = sphere2->GetPosition() - sphere1->GetPosition();
		float distance = glm::length(delta);
		float intersection = sphere1->GetRadius() + sphere2->GetRadius() - distance;
		if (intersection > 0)
		{
			glm::vec2 collisionNormal = glm::normalize(delta);
			glm::vec2 relativeVelocity = sphere1->GetVelocity() - sphere2->GetVelocity();
			glm::vec2 collisionVector = collisionNormal *(glm::dot(relativeVelocity, collisionNormal));
			glm::vec2 forceVector = collisionVector * 1.0f / ((1 / sphere1->GetMass()) + (1 / sphere2->GetMass()));
			//use newtons 3rd law to apply collision forces to colliding bodies
			sphere1->ApplyForceToActor(sphere2, 2 * forceVector);
			sphere1->ApplyForce(2 * -forceVector);
			//move our spheres out of collision
			glm::vec2 seperationVector = collisionNormal * intersection * 0.5f;
			sphere1->SetPosition(sphere1->GetPosition() - seperationVector);
			sphere2->SetPosition(sphere2->GetPosition() + seperationVector);
			return true;
		}
	}
	return false;
}

bool PhysScene::PlaneToSphere(Actor* obj1, Actor* obj2)
{
	//try to cast to sphere and sphere
	Plane* plane = dynamic_cast<Plane*>(obj1);
	Sphere* sphere = dynamic_cast<Sphere*>(obj2);
	//if successful then test collision
	if (plane != NULL && sphere != NULL)
	{
		glm::vec2 collisionNormal = plane->GetNormal();
		float planeToSphere = glm::dot(sphere->GetPosition(), plane->GetNormal()) - plane->GetOffset();
		if (planeToSphere < 0)
		{
			collisionNormal *= -1;
			planeToSphere *= -1;
		}
		float intersection = sphere->GetRadius() - planeToSphere;
		if (intersection > 0)
		{
			glm::vec2 planeNormal = plane->GetNormal();
			if (planeToSphere < 0)
			{
				planeNormal *= -1;
			}
			glm::vec2 forceVector = -1 * sphere->GetMass() * planeNormal * (glm::dot(planeNormal, sphere->GetVelocity()));
			sphere->ApplyForce(2 * forceVector);
			sphere->SetPosition(sphere->GetPosition() + (collisionNormal * intersection * 0.5f));
			return true;
		}
	}
	return false;
}

bool PhysScene::SphereToPlane(Actor* obj1, Actor* obj2)
{
	return PlaneToSphere(obj2, obj1);
}


bool PhysScene::PlaneToPlane(Actor* obj1, Actor* obj2)
{
	//try to cast to sphere and sphere
	Plane* plane1 = dynamic_cast<Plane*>(obj1);
	Plane* plane2 = dynamic_cast<Plane*>(obj2);
	//if successful then test collision
	if (plane1 != NULL && plane2 != NULL)
	{
		//unnecessary check
		return true;
	}
	return false;
}

bool PhysScene::PlaneToBox(Actor* obj1, Actor* obj2)
{
	//try to cast to sphere and sphere
	Plane* plane = dynamic_cast<Plane*>(obj1);
	Box* box = dynamic_cast<Box*>(obj2);
	//if successful then test collision
	if (plane != NULL && box != NULL)
	{
		//check if AABB overlaps the plane
		glm::vec2 collisionNormal = plane->GetNormal();
		float planeToSphere = glm::dot(box->GetPosition(), plane->GetNormal()) - plane->GetOffset();
		if (planeToSphere < 0)
		{
			collisionNormal *= -1;
			planeToSphere *= -1;
		}
		

		return true;
	}
	return false;
}

bool PhysScene::SphereToBox(Actor* obj1, Actor* obj2)
{
	//try to cast to sphere and sphere
	Sphere* sphere = dynamic_cast<Sphere*>(obj1);
	Box* box = dynamic_cast<Box*>(obj2);
	//if successful then test collision
	if (sphere != NULL && box != NULL)
	{
		
		return true;
	}
	return false;
}

bool PhysScene::BoxToPlane(Actor* obj1, Actor* obj2)
{
	return PlaneToBox(obj2, obj1);
}

bool PhysScene::BoxToSphere(Actor* obj1, Actor* obj2)
{
	return SphereToBox(obj2, obj1);
}

bool PhysScene::BoxToBox(Actor* obj1, Actor* obj2)
{
	//try to cast to sphere and sphere
	Box* box1 = dynamic_cast<Box*>(obj1);
	Box* box2 = dynamic_cast<Box*>(obj2);
	//if successful then test collision
	if (box1 != NULL && box2 != NULL)
	{
		//check iff AABB are overlapping
		if (box1->GetPosition().x < box2->GetPosition().x + box2->GetLength() &&
			box1->GetPosition().x + box1->GetLength() > box2->GetPosition().x &&
			box1->GetPosition().y < box2->GetPosition().y + box2->GetHeight() &&
			box1->GetPosition().y + box1->GetHeight() > box2->GetPosition().y)
		{
			//they are intersecting
			glm::vec2 delta = box2->GetPosition() - box1->GetPosition();
			float distance = glm::length(delta);
			float intersection = (box1->GetLength() / 2) + (box2->GetHeight() / 2) - distance; //-------------all this code really needs improving

			glm::vec2 collisionNormal = glm::normalize(delta);
			glm::vec2 relativeVelocity = box1->GetVelocity() - box2->GetVelocity();
			glm::vec2 collisionVector = collisionNormal *(glm::dot(relativeVelocity, collisionNormal));
			glm::vec2 forceVector = collisionVector * 1.0f / ((1 / box1->GetMass()) + (1 / box2->GetMass()));
			//use newtons 3rd law to apply collision forces to colliding bodies
			box1->ApplyForceToActor(box2, 2 * forceVector);
			box1->ApplyForce(2 * -forceVector);
			//move our spheres out of collision
			glm::vec2 seperationVector = collisionNormal * intersection * 0.5f;
			box1->SetPosition(box1->GetPosition() - seperationVector);
			box2->SetPosition(box2->GetPosition() + seperationVector);
			return true;
		}
		return true;
	}
	return false;
}