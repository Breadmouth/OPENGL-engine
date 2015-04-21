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
using glm::vec2;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Destroy();

	//contains updates for all objects handles by renderer
	void Update(float timer, float dt, mat4 *cameraTransform);
	//contains draw for all objects handled by renderer
	void Draw(vec3 *light, vec3* lightColour, mat4 *lightMatrix, 
			  mat4* projectionView, vec3* cameraPos, float* specPow, float* height, float* waterHeight, float* time);

	//load vertex and fragment shader into a program
	bool LoadShader(std::string program, std::string vertex, std::string fragment);
	static GLuint LoadShaderFromFile(std::string path, GLenum shaderType);

	//create buffers for obj
	void CreateOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	//create buffers for FBX
	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanupOpenGLBuffers(FBXFile* fbx);

	//load obj file
	void LoadObject(std::string path);

	//load fbx file
	void LoadFBX(const char* path);
	void UpdateFBX(float timer);

	//create a cpu particle emmitter
	void CreateParticleEmitter();
	void UpdateParticles(float dt, mat4 cameraTransform);

	//create frame buffer & plane for postproccesing effects
	void CreateFB();
	void CreateViewPlane();

	//create frame buffer for shadow map
	void CreateSB();
	//create plane to draw shadow on
	void CreateShadowPlane();

	//create plane to use for terrain gen
	void CreateTerrainPlane(int width, int height);
	//generate a perlin noise
	void CreatePerlin(int dims);
	//generate a diamond square terrain
	void CreateDiamondSquare(int dims);
	//create plane to use as water
	void CreateWaterPlane(int dims);

	vec3 GetTerrainPos(int x, int y);
	vec2 GetTerrainTexCoord(int x, int y);

	void LoadTexture(std::string texture, std::string path, GLint type);
	void LoadNormal(std::string path);
	void LoadSpecular(std::string path);

	void BindFrameBuffer(bool bind);

	bool GetAnimateFBX();
	void SetAnimateFBX(bool animate);
	void SetAnimateModel(int i, bool animate);

	void FillModel(int i, mat4 pos, mat4 rot, mat4 scale);
	void SetModelPos(int i, vec3 pos);
	void SetModelScale(int i, float scale);
	void SetModelHeightTexCoord(int i, vec2 texCoord);
	void SetModelTexture(int i, std::string name);

	struct Vertex {
		float v_x;
		float v_y;
		float v_z;
		float v_w;
		float t_x;
		float t_y;
		vec3 normal;
	};

private:
	unsigned int m_texture;
	unsigned int m_normal;
	unsigned int m_specular;

	struct GLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_indexCount;
	};

	struct Model
	{
		FBXFile* m_fbx;
		unsigned int m_texture;
		unsigned int m_normal;
		unsigned int m_specular;
		mat4 m_position;
		mat4 m_rotation;
		mat4 m_scale;
		glm::vec2 m_heightTexCoord;
		bool m_animate;
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
	GLInfo m_terrainPlane;
	GLInfo m_waterPlane;
	GLInfo m_shadowPlane;

	vec3 **m_terrain;
	vec3 **m_terrainNormals;
	glm::vec2 **m_terrainTexCoords;

	unsigned int m_perlin_texture;
	unsigned int m_rock_texture;
	unsigned int m_grass_texture;
	unsigned int m_snow_texture;
	unsigned int m_water_texture;
	unsigned int m_sand_texture;

	unsigned int m_water_height_1;
	unsigned int m_water_height_2;

	//FBXFile* m_fbx;
	Model m_models[10];
	ParticleEmitter* m_emitter;

	unsigned int m_programID;
	unsigned int m_postProcessProgram;
	unsigned int m_shadowProgram;
	unsigned int m_shadowProgramAnim;
	unsigned int m_shadowGenProgram;
	unsigned int m_shadowGenProgramAnim;
	unsigned int m_terrainGenProgram;
	unsigned int m_terrainGenShadowProgram;
	unsigned int m_waterProgram;

	unsigned int m_objects;

	bool m_animateFBX;
};