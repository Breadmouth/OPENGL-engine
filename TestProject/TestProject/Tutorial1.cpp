#include "Tutorial1.h"
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

void Tutorial1::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	planet1 = vec3(10, 0, 0);
	planet1Rot = 0;
}

void Tutorial1::Destroy()
{

}

void Tutorial1::Update(float dt)
{
	//if (planet1Rot > 2 * 3.14159265)
	//	planet1Rot = 0;
	

	planet1Rot += dt;

	mat4 planet1Translate;
	planet1Translate[0] = vec4(glm::cos(planet1Rot), 0, glm::sin(planet1Rot), 0);
	planet1Translate[1] = vec4(0, 1, 0, 0);
	planet1Translate[2] = vec4(-glm::sin(planet1Rot), 0, glm::cos(planet1Rot), 0);
	planet1Translate[3] = vec4(0, 0, 0, 1);

	planet1.x = 10 * glm::sin(2 * 3.1415 * (planet1Rot * 0.2));
	planet1.z = 10 * glm::cos(2 * 3.1415 * (planet1Rot * 0.2));

	Gizmos::addSphere(vec3(0,0,0), 2, 15, 15, vec4(0.5f, 0.5f, 0, 1));
	Gizmos::addSphere(planet1, 1, 12, 12, vec4(0.8f, 0.f, 0.2f, 1), &planet1Translate);


	camera.Update(dt);
}

void Tutorial1::Draw()
{
	Gizmos::draw(camera.GetProjection() * camera.GetView());
}