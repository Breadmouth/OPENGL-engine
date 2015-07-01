#include "Actors.h"


//----------Plane
Plane::Plane(glm::vec3 normal, float offset, glm::vec4 colour)
{
	m_offset = offset;
	m_normal = normal;
	m_colour = colour;
	m_shapeID = ShapeType::PLANE;
}

void Plane::MakeGizmo()
{
	//Gizmos::addAABBFilled(centre, glm::vec3(1000, 1000, 0.01), m_colour, &angle);
}


//---------RigidBody
RigidBody::RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass, bool isStatic)
{
	m_position = position;
	m_velocity = velocity;
	m_mass = mass;
	m_rotation2D = rotation;
	m_static = isStatic;

	m_angularVelocity = glm::vec3(0);
	m_angularDrag = 1.f;
	m_linearDrag = 0.992f;
}

void RigidBody::Update(glm::vec3 gravity, float timeStep)
{
	if (!m_static)
	{
		m_position += m_velocity * timeStep;
		m_velocity *= m_linearDrag;
		//ApplyForce(glm::vec2(gravity.x, gravity.y));
	}
}

void RigidBody::ApplyForce(glm::vec3 force)
{
	if (!m_static)
	{
		glm::vec3 accel;
		accel = force / m_mass;
		m_velocity += accel;
	}
}

void RigidBody::ApplyForceToActor(RigidBody* actor2, glm::vec3 force)
{
	actor2->ApplyForce(force);
}

void RigidBody::SetVelocity(glm::vec3 velocity)
{
	m_velocity = velocity;
}

void RigidBody::SetPosition(glm::vec3 position)
{
	m_position = position;
}

//----------Sphere
Sphere::Sphere(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour, bool isStatic)
: RigidBody(position, velocity, 0, mass, isStatic)
{
	m_radius = radius;
	m_colour = colour;
	m_shapeID = ShapeType::SPHERE;
}

void Sphere::MakeGizmo()
{
	Gizmos::addSphere(m_position, m_radius, 10, 10, m_colour);
}

//--------Box
Box::Box(glm::vec3 position, glm::vec3 velocity, float mass, float height, float length, float width, glm::vec4 colour, bool isStatic)
: RigidBody(position, velocity, 0, mass, isStatic)
{
	m_height = height;
	m_length = length;
	m_width = width;
	m_colour = colour;
	m_shapeID = ShapeType::BOX;
}

void Box::MakeGizmo()
{
	Gizmos::addAABBFilled(m_position, glm::vec3(m_length / 2, m_height / 2, 1), m_colour);
}