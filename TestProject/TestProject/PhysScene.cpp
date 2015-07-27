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
	PhysScene::PlaneToPlane,	//done
	PhysScene::PlaneToSphere,	//done
	PhysScene::PlaneToBox,		//done
	PhysScene::PlaneToJoint,	//ignore
	PhysScene::SphereToPlane,	//done
	PhysScene::SphereToSphere,	//done
	PhysScene::SphereToBox,		//done
	PhysScene::SphereToJoint,	//ignore
	PhysScene::BoxToPlane,		//done
	PhysScene::BoxToSphere,		//done
	PhysScene::BoxToBox,		//done
	PhysScene::BoxToJoint,		//ignore
	PhysScene::JointToPlane,	//ignore
	PhysScene::JointToSphere,	//ignore
	PhysScene::JointToBox,		//ignore
	PhysScene::JointToJoint,	//ignore
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

		glm::vec3 delta = sphere2->GetPosition() - sphere1->GetPosition();
		float distance = glm::length(delta);
		float intersection = sphere1->GetRadius() + sphere2->GetRadius() - distance;
		if (intersection > 0)
		{
			glm::vec3 collisionNormal = glm::normalize(delta);
			glm::vec3 relativeVelocity = sphere1->GetVelocity() - sphere2->GetVelocity();

			if (relativeVelocity.x == 0 && relativeVelocity.y == 0 && relativeVelocity.z == 0)
			{
				relativeVelocity = glm::vec3(1, 0, 0);
			}
			glm::vec3 collisionVector = collisionNormal *(glm::dot(relativeVelocity, collisionNormal));

			glm::vec3 V2 = collisionNormal * sphere1->GetRadius();
			if (!sphere1->GetStatic() && !sphere2->GetStatic())
			{
				glm::vec3 forceVector = collisionVector * 1.0f / ((1 / sphere1->GetMass()) + (1 / sphere2->GetMass()));

				//apply rotational velocity
				glm::vec3 torqueLever = glm::normalize(glm::cross(glm::cross(relativeVelocity, V2), V2)) *  sphere1->GetRadius();
				float torque = glm::dot(torqueLever, relativeVelocity) * 1.0f / (1 / sphere1->GetMass() + 1 / sphere2->GetMass());
				glm::vec3 dir = glm::normalize(glm::cross(V2, relativeVelocity));
				sphere1->ApplyTorque(-torque, dir);
				sphere2->ApplyTorque(torque, dir);

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (sphere1->GetElasticity() + sphere2->GetElasticity()) / 2.0f;
				sphere1->ApplyForceToActor(sphere2, forceVector + (forceVector * combinedElasticity));
				sphere1->ApplyForce(-forceVector - (forceVector * combinedElasticity));

				//move our spheres out of collision
				glm::vec3 seperationVector = collisionNormal * intersection * 0.5f;
				sphere1->SetPosition(sphere1->GetPosition() - seperationVector);
				sphere2->SetPosition(sphere2->GetPosition() + seperationVector);
			}
			else if (sphere1->GetStatic())
			{
				glm::vec3 forceVector = collisionVector * 1.0f / ((0.0000001f) + (1 / sphere2->GetMass()));

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (sphere1->GetElasticity() + sphere2->GetElasticity()) / 2.0f;
				sphere1->ApplyForceToActor(sphere2, forceVector + (forceVector * combinedElasticity));

				//apply rotational velocity
				glm::vec3 torqueLever = glm::normalize(glm::cross(glm::cross(relativeVelocity, V2), V2)) *  sphere1->GetRadius();
				float torque = glm::dot(torqueLever, relativeVelocity) * 1.0f / (1 / sphere1->GetMass() + 1 / sphere2->GetMass());
				glm::vec3 dir = glm::normalize(glm::cross(V2, relativeVelocity));
				sphere2->ApplyTorque(torque, dir);

				//move our spheres out of collision
				glm::vec3 seperationVector = collisionNormal * intersection;
				sphere2->SetPosition(sphere2->GetPosition() + seperationVector);
			}
			else
			{
				glm::vec3 forceVector = collisionVector * 1.0f / ((0.0000001f) + (1 / sphere1->GetMass()));

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (sphere1->GetElasticity() + sphere2->GetElasticity()) / 2.0f;
				sphere2->ApplyForceToActor(sphere1, -forceVector - (forceVector * combinedElasticity));

				//apply rotational velocity
				glm::vec3 torqueLever = glm::normalize(glm::cross(glm::cross(relativeVelocity, V2), V2)) *  sphere1->GetRadius();
				float torque = glm::dot(torqueLever, relativeVelocity) * 1.0f / (1 / sphere1->GetMass() + 1 / sphere2->GetMass());
				glm::vec3 dir = glm::normalize(glm::cross(V2, relativeVelocity));
				sphere1->ApplyTorque(-torque, dir);

				//move our spheres out of collision
				glm::vec3 seperationVector = collisionNormal * intersection;
				sphere1->SetPosition(sphere1->GetPosition() - seperationVector);
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
		glm::vec3 collisionNormal = plane->GetNormal();
		float planeToSphere = glm::dot(sphere->GetPosition(), plane->GetNormal()) - plane->GetOffset();
		if (planeToSphere < 0)
		{
			collisionNormal *= -1;
			planeToSphere *= -1;
		}
		float intersection = sphere->GetRadius() - planeToSphere;
		if (intersection > 0)
		{
			glm::vec3 planeNormal = plane->GetNormal();
			if (planeToSphere < 0)
			{
				planeNormal *= -1;
			}
			glm::vec3 forceVector = -1 * sphere->GetMass() * planeNormal * (glm::dot(planeNormal, sphere->GetVelocity()));
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
		if (!box->GetStatic())
		{
			//check if AABB overlaps the plane
			glm::vec3 collisionNormal = plane->GetNormal();
			float planeToBox = glm::dot(box->GetPosition(), plane->GetNormal()) - plane->GetOffset();
			glm::vec3 extents = glm::vec3(box->GetHeight() / 2.0f, box->GetLength() / 2.0f, box->GetWidth() / 2.0f);

			if (planeToBox < 0)
			{
				collisionNormal *= -1;
				planeToBox *= -1;
			}
			float intersection = glm::length((extents * collisionNormal)) - planeToBox;
			if (intersection > 0)
			{
				glm::vec3 planeNormal = plane->GetNormal();
				if (planeToBox < 0)
				{
					planeNormal *= -1;
				}
				glm::vec3 forceVector = -1 * box->GetMass() * planeNormal * (glm::dot(planeNormal, box->GetVelocity()));
				box->ApplyForce(2 * forceVector);
				box->SetPosition(box->GetPosition() + (collisionNormal * intersection * 0.5f));
				return true;
			}


			return true;
		}
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
		glm::vec3 sphereBoxDist = sphere->GetPosition() - box->GetPosition();
		glm::vec3 boxPoint;

		glm::vec3 c1 = glm::vec3(box->GetPosition().x - (box->GetLength() / 2.f), box->GetPosition().y - (box->GetHeight() / 2.f), box->GetPosition().z - (box->GetWidth() / 2.f));
		glm::vec3 c2 = glm::vec3(box->GetPosition().x + (box->GetLength() / 2.f), box->GetPosition().y + (box->GetHeight() / 2.f), box->GetPosition().z + (box->GetWidth() / 2.f));

		float distSq = sphere->GetRadius() * sphere->GetRadius();
		if (sphere->GetPosition().x < c1.x)
			distSq -= pow(sphere->GetPosition().x - c1.x, 2);
		else if (sphere->GetPosition().x > c2.x)
			distSq -= pow(sphere->GetPosition().x - c2.x, 2);
		if (sphere->GetPosition().y < c1.y)
			distSq -= pow(sphere->GetPosition().y - c1.y, 2);
		else if (sphere->GetPosition().y > c2.y)
			distSq -= pow(sphere->GetPosition().y - c2.y, 2);
		if (sphere->GetPosition().z < c1.z)
			distSq -= pow(sphere->GetPosition().z - c1.z, 2);
		else if (sphere->GetPosition().z > c2.z)
			distSq -= pow(sphere->GetPosition().z - c2.z, 2);

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
		
		if (sphereBoxDist.z < -box->GetWidth() / 2.f)
		{
			boxPoint.z = -box->GetWidth() / 2.f;
		}
		else if (sphereBoxDist.z > box->GetWidth() / 2.f)
		{
			boxPoint.z = box->GetWidth() / 2.f;
		}
		else
		{
			boxPoint.z = sphereBoxDist.z;
		}

		if (distSq > 0)
		{
			glm::vec3 delta = sphere->GetPosition() - box->GetPosition();
			float intersection;

			glm::vec3 boxNormal;
			float modHL = box->GetHeight() / box->GetLength();
			float modHW = box->GetHeight() / box->GetWidth();
			float modLW = box->GetLength() / box->GetWidth();
			if (delta.y > 0 && (delta.x < delta.y / modHL && delta.x > -delta.y / modHL) && (delta.z < delta.y / modHW && delta.z > -delta.y / modHW)) //top
			{
				boxNormal = glm::vec3(0, 1, 0);
				intersection = sphere->GetRadius() + glm::length(boxPoint.y) - (glm::length(sphere->GetPosition() - glm::vec3(sphere->GetPosition().x, box->GetPosition().y, box->GetPosition().z)));
			}
			else if (delta.y < 0 && (delta.x > delta.y / modHL && delta.x < -delta.y / modHL) && (delta.z > delta.y / modHW && delta.z < -delta.y / modHW)) //bottom
			{
				boxNormal = glm::vec3(0, -1, 0);
				intersection = sphere->GetRadius() + glm::length(boxPoint.y) - (glm::length(sphere->GetPosition() - glm::vec3(sphere->GetPosition().x, box->GetPosition().y, box->GetPosition().z)));
			}
			else if (delta.x > 0 && (delta.y < delta.x * modHL && delta.y > -delta.x * modHL) && (delta.z < delta.x / modLW && delta.z > -delta.x / modLW)) //right
			{
				boxNormal = glm::vec3(1, 0, 0);
				intersection = sphere->GetRadius() + glm::length(boxPoint.x) - (glm::length(sphere->GetPosition() - glm::vec3(box->GetPosition().x, sphere->GetPosition().y, box->GetPosition().z)));
			}
			else if (delta.x < 0 && (delta.y > delta.x * modHL && delta.y < -delta.x * modHL) && (delta.z > delta.x / modLW && delta.z < -delta.x / modLW)) //left
			{
				boxNormal = glm::vec3(-1, 0, 0);
				intersection = sphere->GetRadius() + glm::length(boxPoint.x) - (glm::length(sphere->GetPosition() - glm::vec3(box->GetPosition().x, sphere->GetPosition().y, box->GetPosition().z)));
			}
			else if (delta.z > 0 && (delta.y < delta.z * modHW && delta.y > -delta.z * modHW) && (delta.x < delta.z * modLW && delta.x > -delta.z * modLW)) //front
			{
				boxNormal = glm::vec3(0, 0, 1);
				intersection = sphere->GetRadius() + glm::length(boxPoint.z) - (glm::length(sphere->GetPosition() - glm::vec3(box->GetPosition().x, sphere->GetPosition().y, box->GetPosition().z)));
			}
			else if (delta.z < 0 && (delta.y > delta.z * modHW && delta.y < -delta.z * modHW) && (delta.x > delta.z * modLW && delta.x < -delta.z * modLW)) //back
			{
				boxNormal = glm::vec3(0, 0, -1);
				intersection = sphere->GetRadius() + glm::length(boxPoint.z) - (glm::length(sphere->GetPosition() - glm::vec3(box->GetPosition().x, sphere->GetPosition().y, box->GetPosition().z)));
			}

			glm::vec3 relativeVelocity = sphere->GetVelocity() - box->GetVelocity();
			glm::vec3 collisionVector = boxNormal * (glm::dot(boxNormal, relativeVelocity));

			if (!sphere->GetStatic() && !box->GetStatic())
			{
				//glm::vec3 forceVector = -1 * sphere->GetMass() * boxNormal * (glm::dot(boxNormal, sphere->GetVelocity()));
				glm::vec3 forceVector = -1 * collisionVector * 1.0f / ((1 / sphere->GetMass()) + (1 / box->GetMass()));

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (sphere->GetElasticity() + box->GetElasticity()) / 2.0f;
				box->ApplyForceToActor(sphere, forceVector + (forceVector * combinedElasticity));
				box->ApplyForce(-forceVector - (forceVector * combinedElasticity));

				//move our spheres out of collision
				glm::vec3 seperationVector = boxNormal * intersection * 0.5f;
				sphere->SetPosition(sphere->GetPosition() + seperationVector);
				box->SetPosition(box->GetPosition() - seperationVector);

			}
			else if (sphere->GetStatic())
			{
				//glm::vec3 forceVector = -1 * sphere->GetMass() * boxNormal * (glm::dot(boxNormal, sphere->GetVelocity()));
				glm::vec3 forceVector = -1 * collisionVector * 1.0f / ((1 / sphere->GetMass()) + (1 / box->GetMass()));

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (sphere->GetElasticity() + box->GetElasticity()) / 2.0f;
				box->ApplyForce(-forceVector - (forceVector * combinedElasticity));

				//move our spheres out of collision
				glm::vec3 seperationVector = boxNormal * intersection * 0.5f;
				box->SetPosition(box->GetPosition() - seperationVector);
			}
			else
			{
				//glm::vec3 forceVector = -1 * sphere->GetMass() * boxNormal * (glm::dot(boxNormal, sphere->GetVelocity()));
				glm::vec3 forceVector = -1 * collisionVector * 1.0f / ((1 / sphere->GetMass()) + 0.00001f);

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (sphere->GetElasticity() + box->GetElasticity()) / 2.0f;
				box->ApplyForceToActor(sphere, forceVector + (forceVector * combinedElasticity));

				//move our spheres out of collision
				glm::vec3 seperationVector = boxNormal * intersection * 0.5f;
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
		if (box1->GetStatic() && box2->GetStatic())
			return false;

		//check iff AABB are overlapping
		if (box1->GetPosition().x - box1->GetLength() / 2.f < box2->GetPosition().x + box2->GetLength() / 2.f &&
			box1->GetPosition().x + box1->GetLength() / 2.f > box2->GetPosition().x - box2->GetLength() / 2.f &&
			box1->GetPosition().y - box1->GetHeight() / 2.f < box2->GetPosition().y + box2->GetHeight() / 2.f &&
			box1->GetPosition().y + box1->GetHeight() / 2.f > box2->GetPosition().y - box2->GetHeight() / 2.f &&
			box1->GetPosition().z - box1->GetWidth() / 2.f < box2->GetPosition().z + box2->GetWidth() / 2.f &&
			box1->GetPosition().z + box1->GetWidth() / 2.f > box2->GetPosition().z - box2->GetWidth() / 2.f)
		{
			glm::vec3 delta = box2->GetPosition() - box1->GetPosition();

			float intersection;

			//find boxnormal/collision normal
			glm::vec3 boxNormal;
			float modHL = box2->GetHeight() / box2->GetLength();
			float modHW = box2->GetHeight() / box2->GetWidth();
			float modLW = box2->GetLength() / box2->GetWidth();
			if (delta.y > 0 && (delta.x < delta.y / modHL && delta.x > -delta.y / modHL) && (delta.z < delta.y / modHW && delta.z > -delta.y / modHW)) //top
			{
				boxNormal = glm::vec3(0, 1, 0);
				intersection = delta.y - (box1->GetHeight() / 2.0f) - (box2->GetHeight() / 2.0f);
			}
			else if (delta.y < 0 && (delta.x > delta.y / modHL && delta.x < -delta.y / modHL) && (delta.z > delta.y / modHW && delta.z < -delta.y / modHW)) //bottom
			{
				boxNormal = glm::vec3(0, -1, 0);
				intersection = delta.y + (box1->GetHeight() / 2.0f) + (box2->GetHeight() / 2.0f);
			}
			else if (delta.x > 0 && (delta.y < delta.x * modHL && delta.y > -delta.x * modHL) && (delta.z < delta.x / modLW && delta.z > -delta.x / modLW)) //right
			{
				boxNormal = glm::vec3(1, 0, 0);
				intersection = delta.x - (box1->GetLength() / 2.0f) - (box2->GetLength() / 2.0f);
			}
			else if (delta.x < 0 && (delta.y > delta.x * modHL && delta.y < -delta.x * modHL) && (delta.z > delta.x / modLW && delta.z < -delta.x / modLW)) //left
			{
				boxNormal = glm::vec3(-1, 0, 0);
				intersection = delta.x + (box1->GetLength() / 2.0f) + (box2->GetLength() / 2.0f);
			}
			else if (delta.z > 0 && (delta.y < delta.z * modHW && delta.y > -delta.z * modHW) && (delta.x < delta.z * modLW && delta.x > -delta.z * modLW)) //front
			{
				boxNormal = glm::vec3(0, 0, 1);
				intersection = delta.z - (box1->GetWidth() / 2.0f) - (box2->GetWidth() / 2.0f);
			}
			else if (delta.z < 0 && (delta.y > delta.z * modHW && delta.y < -delta.z * modHW) && (delta.x > delta.z * modLW && delta.x < -delta.z * modLW)) //back
			{
				boxNormal = glm::vec3(0, 0, -1);
				intersection = delta.z + (box1->GetWidth() / 2.0f) + (box2->GetWidth() / 2.0f);
			}

			float distance = glm::length(delta);
			glm::vec3 relativeVelocity = box1->GetVelocity() - box2->GetVelocity();

			glm::vec3 collisionVector = boxNormal *(glm::dot(relativeVelocity, boxNormal));

			if (!box1->GetStatic() && !box2->GetStatic())
			{
				glm::vec3 forceVector = collisionVector * 1.0f / ((1 / box1->GetMass()) + (1 / box2->GetMass()));

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (box1->GetElasticity() + box2->GetElasticity()) / 2.0f;
				box1->ApplyForceToActor(box2, forceVector + (forceVector * combinedElasticity));
				box1->ApplyForce(-forceVector - (forceVector * combinedElasticity));

				//move our bodies out of collision
				glm::vec3 seperationVector = boxNormal * intersection * 0.5f;
				box1->SetPosition(box1->GetPosition() + seperationVector);
				box2->SetPosition(box2->GetPosition() - seperationVector);
			}
			else if (box1->GetStatic() && box2->GetStatic())
			{
			}
			else if (box1->GetStatic())
			{
				glm::vec3 forceVector = collisionVector * 1.0f / (0.00001f + (1 / box2->GetMass()));

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (box1->GetElasticity() + box2->GetElasticity()) / 2.0f;
				box1->ApplyForceToActor(box2, forceVector + (forceVector * combinedElasticity));

				//move our bodies out of collision
				glm::vec3 seperationVector = boxNormal * intersection * 0.5f;
				box2->SetPosition(box2->GetPosition() - seperationVector);
			}
			else //box2 is static
			{
				glm::vec3 forceVector = collisionVector * 1.0f / ((1 / box1->GetMass()) + 0.00001f);

				//use newtons 3rd law to apply collision forces to colliding bodies
				float combinedElasticity = (box1->GetElasticity() + box2->GetElasticity()) / 2.0f;
				box1->ApplyForce(-forceVector - (forceVector * combinedElasticity));

				//move our bodies out of collision
				glm::vec3 seperationVector = boxNormal * intersection * 0.5f;
				box1->SetPosition(box1->GetPosition() + seperationVector);
			}
			return true;
		}
		return true;
	}
	return false;
}

//-------------ALL JOINT CHECKS ARE IGNORED

bool PhysScene::PlaneToJoint(Actor* obj1, Actor* obj2)
{
	return false;
}

bool PhysScene::SphereToJoint(Actor* obj1, Actor* obj2)
{
	return false;
}

bool PhysScene::BoxToJoint(Actor* obj1, Actor* obj2)
{
	return false;
}

bool PhysScene::JointToPlane(Actor* obj1, Actor* obj2)
{
	return false;
}

bool PhysScene::JointToSphere(Actor* obj1, Actor* obj2)
{
	return false;
}

bool PhysScene::JointToBox(Actor* obj1, Actor* obj2)
{
	return false;
}

bool PhysScene::JointToJoint(Actor* obj1, Actor* obj2)
{
	return false;
}
