#pragma once
#include "gl_core_4_4.h"
#include "tiny_obj_loader.h"
#include "FBXFile.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string.h>
#include <vector>
#include <map>
#include <string>
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

	bool LoadShader(std::string program, std::string vertex, std::string fragment);
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

	void CreateFB();
	void CreateViewPlane();

	void CreateSB();
	void CreateShadowPlane();
	void RenderShadowMap(vec3 *light, mat4 *lightMatrix);
	void DrawShadowMap(mat4* projectionView, mat4 *lightMatrix, vec3 *light);

	void CreatePlane();
	void DrawPlane(mat4* projectionView);
	void CreatePerlin();

	void LoadTexture(std::string texture, std::string path, GLint type);
	void LoadNormal(std::string path);

	void DrawOBJ(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow);
	void DrawFBX(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow);
	void DrawFBXGeometry();

	void BindFrameBuffer(bool bind);
	void DrawViewPlane(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow);

	bool GetAnimateFBX();
	void SetAnimateFBX(bool animate);

	struct Vertex {
		float v_x;
		float v_y;
		float v_z;
		float v_w;
		float t_x;
		float t_y;
	};

private:
	unsigned int m_texture, m_normal;

	struct GLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_indexCount;
	};

	std::map<std::string, unsigned int*> m_programs;
	std::map<std::string, unsigned int*> m_textures;

	GLuint m_fbo;
	GLuint m_fboTexture;
	GLuint m_fboDepth;

	GLuint m_sbo;
	GLuint m_sboDepth;

	std::vector<GLInfo> m_glInfo;
	GLInfo m_viewPlane;
	GLInfo m_plane;
	GLInfo m_shadowPlane;

	unsigned int m_perlin_texture;
	unsigned int m_rock_texture;
	unsigned int m_grass_texture;
	unsigned int m_snow_texture;

	FBXFile* m_fbx;
	ParticleEmitter* m_emitter;

	unsigned int m_programID;
	unsigned int m_postProcessProgram;
	unsigned int m_shadowProgram;
	unsigned int m_shadowGenProgram;
	unsigned int m_perlinProgram;

	unsigned int m_objects;

	bool m_animateFBX;
};