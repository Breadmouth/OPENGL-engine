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

		//check if outside bounds of pool table

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
			if (!sphere1->GetStatic() && !sphere2->GetStatic())
			{
				glm::vec2 forceVector = collisionVector * 1.0f / ((1 / sphere1->GetMass()) + (1 / sphere2->GetMass()));
				//use newtons 3rd law to apply collision forces to colliding bodies
				sphere1->ApplyForceToActor(sphere2, 2 * forceVector);
				sphere1->ApplyForce(2 * -forceVector);
				//move our spheres out of collision
				glm::vec2 seperationVector = collisionNormal * intersection * 0.5f;
				sphere1->SetPosition(sphere1->GetPosition() - seperationVector);
				sphere2->SetPosition(sphere2->GetPosition() + seperationVector);
			}
			else if (sphere1->GetStatic())
			{
				glm::vec2 forceVector = collisionVector * 1.0f / ((0.0000001f) + (1 / sphere2->GetMass()));
				//use newtons 3rd law to apply collision forces to colliding bodies
				sphere1->ApplyForceToActor(sphere2, 2 * forceVector);
				//move our spheres out of collision
				glm::vec2 seperationVector = collisionNormal * intersection;
				sphere2->SetPosition(sphere2->GetPosition() - seperationVector);
			}
			else
			{
				glm::vec2 forceVector = collisionVector * 1.0f / ((0.0000001f) + (1 / sphere1->GetMass()));
				//use newtons 3rd law to apply collision forces to colliding bodies
				sphere2->ApplyForceToActor(sphere1, 2 * -forceVector);
				//move our spheres out of collision
				glm::vec2 seperationVector = collisionNormal * intersection;
				sphere1->SetPosition(sphere1->GetPosition() + seperationVector);
			}
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
		glm::vec2 sphereBoxDist = sphere->GetPosition() - box->GetPosition();
		glm::vec2 boxPoint;

		if (sphereBoxDist.x < -box->GetLength() / 2.f)
		{
			boxPoint.x = -box->GetLength() / 2.f;
		}
		else if (sphereBoxDist.x > box->GetLength() / 2.f)
		{
			boxPoint.x = box->GetLength() / 2.f;
		}
		else
		{
			boxPoint.x = sphereBoxDist.x;
		}

		if (sphereBoxDist.y < -box->GetHeight() / 2.f)
		{
			boxPoint.y = -box->GetHeight() / 2.f;
		}
		else if (sphereBoxDist.y > box->GetHeight() / 2.f)
		{
			boxPoint.y = box->GetHeight() / 2.f;
		}
		else
		{
			boxPoint.y = sphereBoxDist.y;
		}

		sphereBoxDist = sphereBoxDist - boxPoint;
		if ((sphereBoxDist.x * sphereBoxDist.x) + (sphereBoxDist.y * sphereBoxDist.y) < sphere->GetRadius() * sphere->GetRadius())
		{
			glm::vec2 delta = sphere->GetPosition() - box->GetPosition();
			float intersection;

			glm::vec2 boxNormal;
			float mod = box->GetHeight() / box->GetLength();
			if (delta.y > 0 && (delta.x < delta.y / mod && delta.x > -delta.y / mod)) //top
			{
				boxNormal = glm::vec2(0, 1);
				intersection = sphere->GetRadius() + glm::length(boxPoint.y) - (glm::length(sphere->GetPosition() - glm::vec2(sphere->GetPosition().x, box->GetPosition().y)));
			}
			if (delta.y < 0 && (delta.x > delta.y / mod && delta.x < -delta.y / mod)) //bottom
			{
				boxNormal = glm::vec2(0, -1);
				intersection = sphere->GetRadius() + glm::length(boxPoint.y) - (glm::length(sphere->GetPosition() - glm::vec2(sphere->GetPosition().x, box->GetPosition().y)));
			}
			if (delta.x > 0 && (delta.y < delta.x * mod && delta.y > -delta.x * mod)) //right
			{
				boxNormal = glm::vec2(1, 0);
				intersection = sphere->GetRadius() + glm::length(boxPoint.x) - (glm::length(sphere->GetPosition() - glm::vec2(box->GetPosition().x, sphere->GetPosition().y)));
			}
			if (delta.x < 0 && (delta.y > delta.x * mod && delta.y < -delta.x * mod)) //left
			{
				boxNormal = glm::vec2(-1, 0);
				intersection = sphere->GetRadius() + glm::length(boxPoint.x) - (glm::length(sphere->GetPosition() - glm::vec2(box->GetPosition().x, sphere->GetPosition().y)));
			}

			glm::vec2 collisionNormal = glm::normalize(delta);
			glm::vec2 relativeVelocity = sphere->GetVelocity() - box->GetVelocity();
			glm::vec2 collisionVector = collisionNormal *(glm::dot(relativeVelocity, collisionNormal));

			if (!sphere->GetStatic() && !box->GetStatic())
			{
				glm::vec2 forceVector = collisionVector * 1.0f / ((1 / sphere->GetMass()) + (1 / box->GetMass()));
				//use newtons 3rd law to apply collision forces to colliding bodies
				sphere->ApplyForceToActor(box, 2 * forceVector);
				sphere->ApplyForce(2 * -forceVector);
				//move our spheres out of collision
				glm::vec2 seperationVector = collisionNormal * intersection * 0.5f;
				sphere->SetPosition(sphere->GetPosition() + seperationVector);
				box->SetPosition(box->GetPosition() - seperationVector);
			}
			else if (sphere->GetStatic())
			{
				glm::vec2 forceVector = collisionVector * 1.0f / ((0.0000001f) + (1 / box->GetMass()));
				//use newtons 3rd law to apply collision forces to colliding bodies
				sphere->ApplyForceToActor(box, 2 * forceVector);
				//move our spheres out of collision
				glm::vec2 seperationVector = collisionNormal * intersection * 0.5f;
				box->SetPosition(box->GetPosition() - seperationVector);
			}
			else
			{
				glm::vec2 forceVector = -1 * sphere->GetMass() * boxNormal * (glm::dot(boxNormal, sphere->GetVelocity()));
				//use newtons 3rd law to apply collision forces to colliding bodies
				box->ApplyForceToActor(sphere, 2 * forceVector);
				//move our spheres out of collision
				glm::vec2 seperationVector = boxNormal * intersection * 0.5f;
				sphere->SetPosition(sphere->GetPosition() + seperationVector);
			}
			return true;
		}
		else
			return false;
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

			return true;
		}
		return true;
	}
	return false;
}