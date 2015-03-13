#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "OBJLoader.h"

#include <vector>

class Tutorial2 : public Application
{
public:
	Tutorial2();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void GenerateGrid( unsigned int rows, unsigned int cols);
	void CreateShader();
	void LoadObject(char *path);

protected:
	FlyCamera camera;

	static struct Vertex
	{
		vec4 position;
		vec4 colour;
	};

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

	unsigned int indexCount;

	OBJLoader *objectLoader;

};