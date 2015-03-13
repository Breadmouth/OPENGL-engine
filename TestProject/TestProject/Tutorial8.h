#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"

#include <vector>

using glm::vec2;

class Tutorial8 : public Application
{
public:
	Tutorial8();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	bool LoadShader(std::string vertex, std::string fragment);

protected:
	GLuint LoadShaderFromFile(std::string path, GLenum shaderType);

	FlyCamera camera;

	Renderer m_renderer;
	ParticleEmitter* m_emitter;

	unsigned int m_programID;

	vec3 light;
	vec3 lightColour;
	float specPow;
	float cameraSpeed;
	bool animate;

	float m_timer;
};