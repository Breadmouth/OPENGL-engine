#pragma once

#include "gl_core_4_4.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <include\GLFW\glfw3.h>

#include "Gizmos.h"

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	BOX = 2,
	NUMBERSHAPE = 3,
};

class Actor
{
public:
	Actor(){};
	~Actor(){};

	void virtual Update(glm::vec3 gravity, float timeStep) = 0;
	void virtual Debug() = 0;
	void virtual MakeGizmo() = 0;
	void virtual ResetPosition(){};

	ShapeType GetShapeType() { return m_shapeID; };

protected:
	ShapeType m_shapeID;
};

class Plane : public Actor
{
public:
	Plane(glm::vec2 normal, float offset, glm::vec4 colour);
	~Plane(){};

	virtual void Update(glm::vec3 gravity, float timeStep){};
	virtual void Debug(){};
	virtual void MakeGizmo();

	glm::vec2 GetNormal(){ return m_normal; };
	float GetOffset(){ return m_offset; };
protected:
	glm::vec2 m_normal;
	float m_offset;
	glm::vec4 m_colour;

};

class RigidBody : public Actor
{
public:
	RigidBody(glm::vec2 position, glm::vec2 velocity, float rotation, float mass);
	~RigidBody(){};

	virtual void Update(glm::vec3 gravity, float timeStep); 

	virtual void Debug(){};

	void ApplyForce(glm::vec2 force);
	void ApplyForceToActor(RigidBody* actor2, glm::vec2 force);

	void ApplyForce();

	void SetVelocity(glm::vec2 velocity);
	void SetPosition(glm::vec2 position);

	glm::vec2 GetPosition(){ return m_position; };
	glm::vec2 GetVelocity(){ return m_velocity; };
	float GetRotation2D(){ return m_rotation2D; };
	float GetMass(){ return m_mass; };

protected:
	glm::vec2 m_position;
	glm::vec2 m_velocity;
	float m_rotation2D;
	float m_mass;

	bool m_static;

	glm::vec4 m_colour;
};

class Sphere : public RigidBody
{
public:
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour);
	~Sphere(){};

	virtual void MakeGizmo();

	float GetRadius(){ return m_radius; };

protected:
	float m_radius;
};

class Box : public RigidBody
{
public:
	Box(glm::vec2 position, glm::vec2 velocity, float mass, float height, float length, glm::vec4 colour);
	~Box(){};
	
	virtual void MakeGizmo();

	float GetHeight(){ return m_height; };
	float GetLength(){ return m_length; };

protected:
	float m_height;
	float m_length;
};