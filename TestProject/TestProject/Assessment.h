#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"

#include <vector>

using glm::vec2;

class Assessment : public Application
{
public:
	Assessment();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

protected:
	FlyCamera camera;

	Renderer m_renderer;
	//GPUParticleEmitter* m_emitter;

	vec3 light;
	vec3 lightColour;
	float specPow;
	float cameraSpeed;
	bool animate;

	bool generate;
	float height;
	vec3 modelPos;

	float m_timer;
	glm::vec2 m_anim;
};