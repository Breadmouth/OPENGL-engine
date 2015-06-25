#include "Actors.h"


//----------Plane
Plane::Plane(glm::vec2 normal, float offset, glm::vec4 colour)
{
	m_offset = offset;
	m_normal = normal;
	m_colour = colour;
	m_shapeID = ShapeType::PLANE;
}

void Plane::MakeGizmo()
{
	glm::vec2 centrePoint = m_offset * m_normal;
	glm::vec3 centre = glm::vec3(centrePoint.x, centrePoint.y, 0);

	float acosy = glm::acos(glm::dot(m_normal.x, m_normal.y) / (glm::length(m_normal.x) - glm::length(m_normal.y)));

	glm::mat4 angle = glm::rotate(acosy, centre);

	//Gizmos::addAABBFilled(centre, glm::vec3(1000, 1000, 0.01), m_colour, &angle);
}


//---------RigidBody
RigidBody::RigidBody(glm::vec2 position, glm::vec2 velocity, float rotation, float mass, bool isStatic)
{
	m_position = position;
	m_velocity = velocity;
	m_mass = mass;
	m_rotation2D = rotation;
	m_static = isStatic;
}

void RigidBody::Update(glm::vec3 gravity, float timeStep)
{
	if (!m_static)
	{
		m_position += m_velocity * timeStep;
		m_velocity *= 0.995f;
		//ApplyForce(glm::vec2(gravity.x, gravity.y));
	}
}

void RigidBody::ApplyForce(glm::vec2 force)
{
	if (!m_static)
	{
		glm::vec2 accel;
		accel.x = force.x / m_mass;
		accel.y = force.y / m_mass;
		m_velocity += accel;
	}
}

void RigidBody::ApplyForceToActor(RigidBody* actor2, glm::vec2 force)
{
	actor2->ApplyForce(force);
}

void RigidBody::SetVelocity(glm::vec2 velocity)
{
	m_velocity = velocity;
}

void RigidBody::SetPosition(glm::vec2 position)
{
	m_position = position;
}

//----------Sphere
Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool isStatic)
: RigidBody(position, velocity, 0, mass, isStatic)
{
	m_radius = radius;
	m_colour = colour;
	m_shapeID = ShapeType::SPHERE;
}

void Sphere::MakeGizmo()
{
	Gizmos::addSphere(glm::vec3(m_position.x, m_position.y, 0), m_radius, 10, 10, m_colour);
}

//--------Box
Box::Box(glm::vec2 position, glm::vec2 velocity, float mass, float height, float length, glm::vec4 colour, bool isStatic)
: RigidBody(position, velocity, 0, mass, isStatic)
{
	m_height = height;
	m_length = length;
	m_colour = colour;
	m_shapeID = ShapeType::BOX;
}

void Box::MakeGizmo()
{
	Gizmos::addAABBFilled(glm::vec3(m_position.x, m_position.y, 0), glm::vec3(m_length / 2, m_height / 2, 1), m_colour);
}