#include "Tutorial10.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial10::Tutorial10()
{
	light = vec3(1, 1, 0);
	lightColour = vec3(1, 1, 1);
	specPow = 16.0f;
	cameraSpeed = camera.GetSpeed();
	animate = false;

	m_timer = 0.0f;
}

void Tutorial10::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());
	camera2.SetIgnoreInput(true);
	camera2.SetLookAt(vec3(0, 1100, -3500), vec3(0, 1100, 0), vec3(0, 1, 0));

	//AntTweakBar
	m_bar = TwNewBar("my bar");
	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "light dir", TW_TYPE_DIR3F, &light, "group=light");
	TwAddVarRW(m_bar, "light colour", TW_TYPE_COLOR3F, &lightColour, "group=light");
	TwAddVarRW(m_bar, "light spec", TW_TYPE_FLOAT, &specPow, "group=light");
	TwAddVarRW(m_bar, "camera speed", TW_TYPE_FLOAT, &cameraSpeed, "group=camera");
	TwAddVarRW(m_bar, "animate", TW_TYPE_BOOLCPP, &animate, "group=animation");

	m_renderer.LoadShader("m_programID", "../data/shaders/fbxShader.glvs", "../data/shaders/fbxShader.glfs");

	m_renderer.LoadFBX("../data/fbx/characters/Enemytank/EnemyTank.fbx");

	m_renderer.LoadTexture("m_texture", "../data/fbx/characters/Enemytank/EnemyTank_D.tga", GL_RGBA);
	m_renderer.LoadNormal("../data/fbx/characters/Enemytank/EnemyTank_N.tga");

	LoadShader();

	CreateFB();

	CreatePlane();

	m_emitter = new GPUParticleEmitter();
	m_emitter->Init(100000, 0.5f, 5.0f, 5, 20, 1, 0, glm::vec4(0.25f, 0, 0.25f, 1), glm::vec4(0, 0.6f, 0.6f, 1));

}

void Tutorial10::Destroy()
{
	m_renderer.Destroy();
}

void Tutorial10::Update(float dt)
{
	if (animate)
		m_timer += dt;

	m_renderer.UpdateFBX(m_timer);

	camera.SetSpeed(cameraSpeed);
	m_renderer.SetAnimateFBX(animate);

	camera.Update(dt);
	camera2.Update(dt);
}

void Tutorial10::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1028, 720);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_renderer.DrawFBX(&camera.GetProjectionView(), &light, &camera.GetPosition(), &lightColour, &specPow);
	m_emitter->Draw((float)glfwGetTime(), camera.GetWorldTransform(), camera.GetProjectionView());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);

	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionView()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glUniform1i(glGetUniformLocation(m_program, "diffuse"), 0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

void Tutorial10::CreateFB()
{
	//setup and bind framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//create texture and bind it
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	//specify texture format for storage
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//attach it to the framebuffer as the first colour attachment
	//the fbo must stil be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

	//setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);

	//while FBO is still bound
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer error!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tutorial10::CreatePlane()
{
	vec2 halfTexel = 1.0f / vec2(1280, 720) * 0.5f;

	float vertexData[] = {
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
	};

	unsigned int indexData[] = {
		0, 2, 1,
		0, 1, 3,
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4,
		vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6,
		indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Tutorial10::LoadShader()
{
	GLuint vertex = m_renderer.LoadShaderFromFile("../data/shaders/tut10.glvs", GL_VERTEX_SHADER);
	GLuint fragment = m_renderer.LoadShaderFromFile("../data/shaders/tut10.glfs", GL_FRAGMENT_SHADER);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertex);
	glAttachShader(m_program, fragment);
	glLinkProgram(m_program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}