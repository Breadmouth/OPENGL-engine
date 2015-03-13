#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "OBJLoader.h"

#include <vector>

using glm::vec2;

class Tutorial4 : public Application
{
public:
	Tutorial4();
	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void GenerateGrid(unsigned int rows, unsigned int cols);

	void CreateShader();
	GLuint LoadShaderFromFile(std::string path, GLenum shaderType);
	bool LoadShader();

	void LoadObject(char *path);
	void CreateSquare();

protected:
	FlyCamera camera;

	static struct Vertex
	{
		vec4 position;
		vec4 normal;
		vec4 tangent;
		vec2 uv;
	};

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

	unsigned int indexCount;

	OBJLoader *objectLoader;

};