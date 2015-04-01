#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"

#include <vector>

using glm::vec2;

class Tutorial13 : public Application
{
public:
	Tutorial13();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

protected:
	FlyCamera camera;

	Renderer m_renderer;
	GPUParticleEmitter* m_emitter;

	//unsigned int m_programID;
	unsigned int m_postProcessProgram;

	vec3 light;
	vec3 lightColour;
	float specPow;
	float cameraSpeed;
	bool animate;

	float m_timer;
};