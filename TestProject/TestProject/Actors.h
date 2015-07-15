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
	Plane(glm::vec3 normal, float offset, glm::vec4 colour);
	~Plane(){};

	virtual void Update(glm::vec3 gravity, float timeStep){};
	virtual void Debug(){};
	virtual void MakeGizmo();

	glm::vec3 GetNormal(){ return m_normal; };
	float GetOffset(){ return m_offset; };
protected:
	glm::vec3 m_normal;
	float m_offset;
	glm::vec4 m_colour;

};

class RigidBody : public Actor
{
public:
	RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass, float elasticity, bool isStatic);
	~RigidBody(){};

	virtual void Update(glm::vec3 gravity, float timeStep); 

	virtual void Debug(){};

	void ApplyForce(glm::vec3 force);
	void ApplyForceToActor(RigidBody* actor2, glm::vec3 force);

	void ApplyForce();

	void SetVelocity(glm::vec3 velocity);
	void SetAngularVelocity(glm::vec3 velocity);
	void SetPosition(glm::vec3 position);
	void SetLinearDrag(float linearDrag) { m_linearDrag = linearDrag; };
	void SetAngularDrag(float angularDrag) { m_angularDrag = angularDrag; };

	glm::vec3 GetPosition(){ return m_position; };
	glm::vec3 GetVelocity(){ return m_velocity; };
	float GetRotation2D(){ return m_rotation2D; };
	float GetMass(){ return m_mass; };
	float GetElasticity() { return m_elasticity; };
	bool GetStatic() { return m_static; };

protected:
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_angularVelocity;

	float m_rotation2D;
	float m_mass;
	float m_elasticity;

	float m_linearDrag;
	float m_angularDrag;

	bool m_static;

	glm::vec4 m_colour;
};

class Sphere : public RigidBody
{
public:
	Sphere(glm::vec3 position, glm::vec3 velocity, float mass, float elasticity, float radius, glm::vec4 colour, bool isStatic);
	~Sphere(){};

	virtual void MakeGizmo();

	float GetRadius(){ return m_radius; };

protected:
	float m_radius;
};

class Box : public RigidBody
{
public:
	Box(glm::vec3 position, glm::vec3 velocity, float mass, float elasticity, float height, float length, float width, glm::vec4 colour, bool isStatic);
	~Box(){};
	
	virtual void MakeGizmo();

	float GetHeight(){ return m_height; };
	float GetLength(){ return m_length; };
	float GetWidth(){ return m_width; };

protected:
	float m_height;
	float m_length;
	float m_width;
};