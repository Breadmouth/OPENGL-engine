#pragma once
#include "Application.h"
#include "FlyCamera.h"

class Tutorial1 : public Application
{
public:
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

protected:
	FlyCamera camera;

	vec3 planet1;
	float planet1Rot;

};