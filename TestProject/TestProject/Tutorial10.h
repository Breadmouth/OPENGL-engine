#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"

#include <vector>

using glm::vec2;

class Tutorial10 : public Application
{
public:
	Tutorial10();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void CreateFB();
	void CreatePlane();
	void LoadShader();


protected:
	FlyCamera camera;
	FlyCamera camera2;

	Renderer m_renderer;
	GPUParticleEmitter* m_emitter;

	GLuint m_fbo;
	GLuint m_fboTexture;
	GLuint m_fboDepth;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int m_program;

	vec3 light;
	vec3 lightColour;
	float specPow;
	float cameraSpeed;
	bool animate;

	float m_timer;
};