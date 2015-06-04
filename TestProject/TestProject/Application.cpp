#include "Application.h"
#include "FlyCamera.h"
#include <gl_core_4_4.h>
#include <include/GLFW/glfw3.h>

#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;


void OnMouseButton(GLFWwindow*, int b, int a, int m){
	TwEventMouseButtonGLFW(b, a);
}
void OnMousePosition(GLFWwindow*, double x, double y){
	TwEventMousePosGLFW((int)x, (int)y);
}
void OnMouseScroll(GLFWwindow*, double x, double y){
	TwEventMouseWheelGLFW((int)y);
}
void OnKey(GLFWwindow*, int k, int s, int a, int m){
	TwEventKeyGLFW(k, a);
}
void OnChar(GLFWwindow*, unsigned int c){
	TwEventCharGLFW(c, GLFW_PRESS);
}
void OnWindowResize(GLFWwindow*, int w, int h){
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}

int Application::Run()
{
	if (glfwInit() == false)
		return -1;

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);


	if (window == nullptr)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	m_clearColour = vec4(0);

	//set up AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);

	//Gizmos::create();

	Create();

	glClearColor( 0.5f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);

	previousDeltaTime = 0;

	while (glfwWindowShouldClose(window) == false &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Gizmos::clear();

		currentTime = (float)glfwGetTime();
		float dt = currentTime - previousDeltaTime;
		previousDeltaTime = currentTime;

		Update(dt);
		Draw();

		TwDraw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Destroy();

	//Gizmos::destroy();

	//destroy AntTweakBar
	TwDeleteAllBars();
	TwTerminate();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
