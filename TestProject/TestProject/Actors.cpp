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
RigidBody::RigidBody(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, float mass, float elasticity, bool isStatic)
{
	m_position = position;
	m_velocity = velocity;
	m_mass = mass;
	m_elasticity = elasticity;
	m_rotation3D = rotation;
	m_static = isStatic;

	m_angularVelocity = glm::vec3(0);
	m_angularDrag = 0.99f;
	m_linearDrag = 0.99f;
}

void RigidBody::Update(glm::vec3 gravity, float timeStep)
{
	if (!m_static)
	{
		m_position += m_velocity * timeStep;
		m_velocity *= m_linearDrag;
		//rotationMatrix = glm::rotate(rotation2D, glm::vec3(0.f, 0.f, 1.0f));
		m_rotation3D += m_angularVelocity * timeStep;
		m_angularVelocity *= m_angularDrag;
		ApplyForce(gravity);

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

void RigidBody::ApplyTorque(float torque, glm::vec3 torqueDir)
{
	if (!m_static)
	{
		m_angularVelocity += torque * torqueDir;
	}
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
Sphere::Sphere(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, float mass, float elasticity, float radius, glm::vec4 colour, bool isStatic)
: RigidBody(position, velocity, rotation, mass, elasticity, isStatic)
{
	m_radius = radius;
	m_colour = colour;
	m_shapeID = ShapeType::SPHERE;
}

void Sphere::MakeGizmo()
{
	glm::mat4 transform = glm::rotate(m_rotation3D.x, glm::vec3(1, 0, 0));
	transform *= glm::rotate(m_rotation3D.y, glm::vec3(0, 1, 0));
	transform *= glm::rotate(m_rotation3D.z, glm::vec3(0, 0, 1));
	Gizmos::addSphere(m_position, m_radius, 10, 10, m_colour, &transform);
}

//--------Box
Box::Box(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, float mass, float elasticity, float height, float length, float width, glm::vec4 colour, bool isStatic)
: RigidBody(position, velocity, rotation, mass, elasticity, isStatic)
{
	m_height = height;
	m_length = length;
	m_width = width;
	m_colour = colour;
	m_shapeID = ShapeType::BOX;
}

void Box::MakeGizmo()
{
	Gizmos::addAABBFilled(m_position, glm::vec3(m_length / 2, m_height / 2, m_width / 2.0f), m_colour);
}