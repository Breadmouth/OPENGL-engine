#include "Tutorial8.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial8::Tutorial8()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;

	m_timer = 0.0f;

}

void Tutorial8::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	//AntTweakBar
	m_bar = TwNewBar("my bar");
	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");

	//m_renderer.LoadShader("../data/shaders/particleShader.glvs", "../data/shaders/particleShader.glfs");
	LoadShader("../data/shaders/particleShader.glvs", "../data/shaders/particleShader.glfs");

	m_emitter = new ParticleEmitter();
	m_emitter->Init(1000, 500, 0.5f, 1.0f, 0.6f, 3.0f, 0.7f, 0.0f, glm::vec4(0, 0, 0, 1), glm::vec4(0, 0.4, 0.4, 1));
	//m_renderer.CreateParticleEmitter();

}

void Tutorial8::Destroy()
{
	m_renderer.Destroy();
}

void Tutorial8::Update(float dt)
{
	if (animate)
		m_timer += dt;

	//m_renderer.UpdateParticles(dt, camera.GetWorldTransform());
	m_emitter->Update(dt, camera.GetWorldTransform());

	camera.SetSpeed(cameraSpeed);

	camera.Update(dt);
}

void Tutorial8::Draw()
{
	glUseProgram(m_programID);
	int projection_view_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionView()));

	m_emitter->Draw();

	//m_renderer.DrawParticles(camera.GetProjectionView());

	Gizmos::draw(camera.GetProjectionView());
}

GLuint Tutorial8::LoadShaderFromFile(std::string path, GLenum shaderType)
{
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile(path.c_str());

	//sourcefile loaded
	if (sourceFile)
	{
		//get shader source
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
		//shaderString.assign();
		//create shaderID
		shaderID = glCreateShader(shaderType);

		//set shader source
		const GLchar* shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);

		//compile shader resource
		glCompileShader(shaderID);

		//check shader for errors
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
		if (shaderCompiled != GL_TRUE)
		{
			printf("unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
			glDeleteShader(shaderID);
			shaderID = 0;
		}
	}
	else
	{
		printf("unable to open file %s\n", path.c_str());
	}

	return shaderID;
}

bool Tutorial8::LoadShader(std::string vertex, std::string fragment)
{
	//Generate program
	m_programID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = LoadShaderFromFile(vertex, GL_VERTEX_SHADER);

	//Check for errors
	if (vertexShader == 0)
	{
		glDeleteProgram(m_programID);
		m_programID = 0;
		return false;
	}

	//Attach vertex shader to program
	glAttachShader(m_programID, vertexShader);


	//Create fragment shader
	GLuint fragmentShader = LoadShaderFromFile(fragment, GL_FRAGMENT_SHADER);

	//Check for errors
	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		glDeleteProgram(m_programID);
		m_programID = 0;
		return false;
	}

	//Attach fragment shader to program
	glAttachShader(m_programID, fragmentShader);

	//Link program
	glLinkProgram(m_programID);

	//Clean up excess shader references
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}