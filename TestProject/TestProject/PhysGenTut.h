#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "tiny_obj_loader.h"
#include "Renderer.h"
#include "FBXFile.h"

#include <vector>

#include "PhysScene.h"

using glm::vec2;

class PhysGenTut : public Application
{
public:
	PhysGenTut();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void SetupTut1();

	void CreatePool();

	void CreateCube();
	void CreateSphere();

	float GetTableWidth() { return tableWidth; };
	float GetTableHeight() { return tableHeight; }; 

	void GetMouseInput();

protected:
	FlyCamera camera;

	GLFWwindow* m_window;

	float tableWidth = 24.f;
	float tableHeight = 12.f;

	bool lMouseButtonPressed = false;
	float cursorX;
	float cursorY;

	float m_createCooldown = 0.f;
	bool m_useMouseInput = false;

	Sphere* m_whiteBall;

	//Renderer m_renderer;
	//GPUParticleEmitter* m_emitter;

	//vec3 light;
	//vec3 lightColour;
	//float specPow;
	//float cameraSpeed;
	//bool animate;
	//
	//bool generate;
	//float height;
	//float waterHeight;
	//vec3 modelPos;
	//float modelScale;
	//
	//float m_timer;
	//glm::vec2 m_anim;

	PhysScene* m_physicsScene;
};