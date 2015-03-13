#pragma once
#include "gl_core_4_4.h"
#include "tiny_obj_loader.h"
#include "FBXFile.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string.h>
#include <vector>
#include "ParticleEmitter.h"
#include "GPUParticleEmitter.h"

using glm::mat4;
using glm::vec3;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Destroy();

	bool LoadShader(std::string vertex, std::string fragment);
	static GLuint LoadShaderFromFile(std::string path, GLenum shaderType);

	//for obj
	void CreateOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	//for FBX
	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanupOpenGLBuffers(FBXFile* fbx);

	void LoadObject(std::string path);

	void LoadFBX(const char* path);
	void UpdateFBX(float timer);

	void CreateParticleEmitter();
	void UpdateParticles(float dt, mat4 cameraTransform);
	void DrawParticles(mat4 cameraProjectionView);

	void LoadTexture(std::string path, GLint type);
	void LoadNormal(std::string path);

	void DrawOBJ(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow);
	void DrawFBX(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow);

	bool GetAnimateFBX();
	void SetAnimateFBX(bool animate);

private:
	unsigned int m_texture, m_normal;

	struct GLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_indexCount;
	};

	std::vector<GLInfo> m_glInfo;

	FBXFile* m_fbx;
	ParticleEmitter* m_emitter;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

	unsigned int indexCount;

	bool m_animateFBX;
};