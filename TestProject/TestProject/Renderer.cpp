#include "Renderer.h"
#include <stb_image.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>

using glm::vec3;

Renderer::Renderer()
{
	srand(time(NULL));

	m_modelCount = 0;

	m_objects = 0;
	m_animateFBX = false;

	m_programs["m_programID"] = &m_programID;
	m_programs["m_postProcessProgram"] = &m_postProcessProgram;
	m_programs["m_shadowProgram"] = &m_shadowProgram;
	m_programs["m_shadowProgramAnim"] = &m_shadowProgramAnim;
	m_programs["m_shadowGenProgram"] = &m_shadowGenProgram;
	m_programs["m_shadowGenProgramAnim"] = &m_shadowGenProgramAnim;
	m_programs["m_terrainGenProgram"] = &m_terrainGenProgram;
	m_programs["m_terrainGenShadowProgram"] = &m_terrainGenShadowProgram;
	m_programs["m_waterProgram"] = &m_waterProgram;

	m_textures["m_texture"] = &m_texture;
	m_textures["m_snow_texture"] = &m_snow_texture;
	m_textures["m_grass_texture"] = &m_grass_texture;
	m_textures["m_rock_texture"] = &m_rock_texture;
	m_textures["m_water_texture"] = &m_water_texture;
	m_textures["m_sand_texture"] = &m_sand_texture;
	m_textures["m_water_height_1"] = &m_water_height_1;
	m_textures["m_water_height_2"] = &m_water_height_2;

}

Renderer::~Renderer()
{

}

void Renderer::Destroy()
{
	for (auto model : m_models)
	{
		if (model->m_fbx != NULL)
			CleanupOpenGLBuffers(model->m_fbx);
	}

	glDeleteProgram(m_programID);
	glDeleteProgram(m_postProcessProgram);
	glDeleteProgram(m_shadowProgram);
	glDeleteProgram(m_shadowProgramAnim);
	glDeleteProgram(m_shadowGenProgram);
	glDeleteProgram(m_shadowGenProgramAnim);
	glDeleteProgram(m_terrainGenProgram);
	glDeleteProgram(m_terrainGenShadowProgram);
	glDeleteProgram(m_waterProgram);
}

void Renderer::LoadObject(std::string path)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, path.c_str());

	CreateOpenGLBuffers(shapes);
}

void Renderer::LoadFBX(const char* path)
{
	Model* newModel = new Model();
	newModel->m_fbx = new FBXFile();
	newModel->m_fbx->load(path);
	CreateOpenGLBuffers(newModel->m_fbx);

	newModel->m_position = mat4{ 1.f };
	newModel->m_rotation = mat4{ 1.f };
	newModel->m_scale = mat4{ 1.f };

	m_models.push_back(newModel);
	m_modelCount++;

	//for (int i = 0; i < 10; ++i)
	//{
	//	if (m_models[i].m_fbx == NULL)
	//	{
	//		m_models[i].m_fbx = new FBXFile();
	//		m_models[i].m_fbx->load(path);
	//		CreateOpenGLBuffers(m_models[i].m_fbx);
	//
	//		m_models[i].m_position = mat4{ 1.f };
	//		m_models[i].m_rotation = mat4{ 1.f };
	//		m_models[i].m_scale = mat4{ 1.f };
	//
	//		return;
	//	}
	//}
}

void Renderer::UpdateFBX(float timer)
{
	for (auto model : m_models)
	{
		if (m_animateFBX)
		{
			FBXSkeleton* skeleton = model->m_fbx->getSkeletonByIndex(0);
			FBXAnimation* animation = model->m_fbx->getAnimationByIndex(0);

			skeleton->evaluate(animation, timer);

			for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
			{
				skeleton->m_nodes[bone_index]->updateGlobalTransform();
			}
		}
	}
	//for (int i = 0; i < 2; ++i)
	//{
	//	if (m_animateFBX)
	//	{
	//		FBXSkeleton* skeleton = m_models[i].m_fbx->getSkeletonByIndex(0);
	//		FBXAnimation* animation = m_models[i].m_fbx->getAnimationByIndex(0);
	//
	//		skeleton->evaluate(animation, timer);
	//
	//		for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	//		{
	//			skeleton->m_nodes[bone_index]->updateGlobalTransform();
	//		}
	//	}
	//}
}

void Renderer::LoadTexture(std::string texture, std::string path, GLint type)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(path.c_str(),
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, m_textures[texture]);
	glBindTexture(GL_TEXTURE_2D, *m_textures[texture]);
	glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, imageWidth, imageHeight, 0, type, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Renderer::LoadNormal(std::string path)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(path.c_str(),
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Renderer::LoadSpecular(std::string path)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(path.c_str(),
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_specular);
	glBindTexture(GL_TEXTURE_2D, m_specular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

GLuint Renderer::LoadShaderFromFile(std::string path, GLenum shaderType)
{
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile(path.c_str());

	//sourcefile loaded
	if (sourceFile)
	{
		//get shader source
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
		//shaderString.assign();
		//create shaderID
		shaderID = glCreateShader(shaderType);

		//set shader source
		const GLchar* shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);

		//compile shader resource
		glCompileShader(shaderID);

		//check shader for errors
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
		if (shaderCompiled != GL_TRUE)
		{
			int bufflen;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &bufflen);
			char* log = new char[bufflen + 1];

			glGetShaderInfoLog(shaderID, bufflen, 0, log);

			printf("unable to compile shader %d!\n\nSource:\n%s\n", shaderID, log);

			delete log;
			glDeleteShader(shaderID);
			shaderID = 0;
		}
	}
	else
	{
		printf("unable to open file %s\n", path.c_str());
	}

	return shaderID;
}

bool Renderer::LoadShader(std::string program, std::string vertex, std::string fragment)
{
	unsigned int* thisProgram = m_programs[program];

	//Generate program
	*thisProgram = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = LoadShaderFromFile(vertex, GL_VERTEX_SHADER);

	//Check for errors
	if (vertexShader == 0)
	{
		glDeleteProgram(*thisProgram);
		thisProgram = 0;
		return false;
	}

	//Attach vertex shader to program
	glAttachShader(*thisProgram, vertexShader);


	//Create fragment shader
	GLuint fragmentShader = LoadShaderFromFile(fragment, GL_FRAGMENT_SHADER);

	//Check for errors
	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		glDeleteProgram(*thisProgram);
		thisProgram = 0;
		return false;
	}

	//Attach fragment shader to program
	glAttachShader(*thisProgram, fragmentShader);

	//Link program
	glLinkProgram(*thisProgram);

	//Clean up excess shader references
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void Renderer::CreateOpenGLBuffers(std::vector < tinyobj::shape_t>& shapes)
{
	m_glInfo.resize(shapes.size());

	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		m_objects += 1;

		glGenVertexArrays(1, &m_glInfo[mesh_index].m_VAO);
		glGenBuffers(1, &m_glInfo[mesh_index].m_VBO);
		glGenBuffers(1, &m_glInfo[mesh_index].m_IBO);
		glBindVertexArray(m_glInfo[mesh_index].m_VAO);

		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();

		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.positions.begin(),
			shapes[mesh_index].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.normals.begin(),
			shapes[mesh_index].mesh.normals.end());

		for (unsigned int j = 1; j < shapes[mesh_index].mesh.texcoords.size(); j += 2)
		{
			shapes[mesh_index].mesh.texcoords[j] = -shapes[mesh_index].mesh.texcoords[j];
		}

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.texcoords.begin(),
			shapes[mesh_index].mesh.texcoords.end());

		m_glInfo[mesh_index].m_indexCount =
			shapes[mesh_index].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glInfo[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0,
			(void*)((sizeof(float)*shapes[mesh_index].mesh.positions.size()) + (sizeof(float)*shapes[mesh_index].mesh.normals.size())));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Renderer::CreateOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::PositionOffset);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Renderer::CleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

bool Renderer::GetAnimateFBX()
{
	return m_animateFBX;
}

void Renderer::SetAnimateFBX(bool animate)
{
	m_animateFBX = animate;
}

void Renderer::SetAnimateModel(int i, bool animate)
{
	m_models[i]->m_animate = animate;
}

void Renderer::CreateParticleEmitter()
{
	m_emitter = new ParticleEmitter();
	m_emitter->Init(1000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));
}

void Renderer::UpdateParticles(float dt, mat4 cameraTransform)
{
	m_emitter->Update(dt, cameraTransform);
}

void Renderer::CreateFB()
{
	//setup and bind framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//create texture and bind it
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	//specify texture format for storage
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//attach it to the framebuffer as the first colour attachment
	//the fbo must stil be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

	//setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);

	//while FBO is still bound
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer error!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateSB()
{
	glGenFramebuffers(1, &m_sbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_sbo);

	glGenTextures(1, &m_sboDepth);
	glBindTexture(GL_TEXTURE_2D, m_sboDepth);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 4096, 4096,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);

	//attach depth to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_sboDepth, 0);

	glDrawBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("FrameBuffer error!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateShadowPlane()
{
	float vertexData[] = {
		-5000, -1, 5000, 1, 0, 1, 0, 1,
		5000, -1, 5000, 1, 0, 1, 0, 1,
		5000, -1, -5000, 1, 0, 1, 0, 1,
		-5000, -1, -5000, 1, 0, 1, 0, 1,
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &m_shadowPlane.m_VAO);
	glBindVertexArray(m_shadowPlane.m_VAO);

	glGenBuffers(1, &m_shadowPlane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_shadowPlane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 8 * 4,
		vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_shadowPlane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shadowPlane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6,
		indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 8, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 8, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::CreateViewPlane()
{
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f;

	float vertexData[] = {
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
	};

	unsigned int indexData[] = {
		0, 2, 1,
		0, 1, 3,
	};

	m_viewPlane.m_indexCount = 6;

	glGenVertexArrays(1, &m_viewPlane.m_VAO);
	glBindVertexArray(m_viewPlane.m_VAO);

	glGenBuffers(1, &m_viewPlane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_viewPlane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4,
		vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_viewPlane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_viewPlane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6,
		indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::BindFrameBuffer(bool bind)
{
	if (bind)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, 1280, 720);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1280, 720);

		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

void Renderer::CreateTerrainPlane(int width, int height)
{
	//m_terrain = new vec3 *[width];
	m_terrainNormals = new vec3*[width];
	m_terrainTexCoords = new glm::vec2 *[width];
	for (int i = 0; i < width; i++)
	{
		//m_terrain[i] = new vec3[height];
		m_terrainNormals[i] = new vec3[width];
		m_terrainTexCoords[i] = new glm::vec2[height];
	}

	std::vector<Vertex> vertexData;

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			Vertex point;
			point.v_x = (float)(-(width * 100) / 2) + (i * 100);
			point.v_y = m_terrain[i][j].y;
			point.v_z = (float)(-(height * 100) / 2) + (j * 100);
			point.v_w = 1;
			point.t_x = 1.0f / width * i;
			point.t_y = 1.0f / height * j;

			vertexData.push_back(point);

			m_terrain[i][j] = vec3(point.v_x, point.v_y, point.v_z);
			m_terrainTexCoords[i][j] = glm::vec2(point.t_x, point.t_y);

		}
	}

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; j++)
		if (i + 1 != width && j + 1 != height)
		{
			vec3 a = m_terrain[i][j];
			vec3 b = m_terrain[i + 1][j];
			vec3 c = m_terrain[i][j + 1];

			vec3 U = b - a;
			vec3 V = c - a;
			
			vertexData[i * width + j].normal.x = (U.y * (V.z/10)) - ((U.z/10) * V.y);
			vertexData[i * width + j].normal.y = -(((U.z/10) * (V.x/10)) - ((U.x/10) * (V.z/10)));
			vertexData[i * width + j].normal.z = ((U.x/10) * V.y) - (U.y * (V.x/10));
			//vec3 normal = glm::cross(b - a, c - a);
			vertexData[i * width + j].normal = glm::normalize(vertexData[i * width + j].normal);
		}
		else
			vertexData[i].normal = vec3(0.f, 1.0f, 0.f);
	}

	unsigned int size = (((width - 1) * 6) * height) - (6 * (width - 1));
	//unsigned int indexData[size];
	std::vector<unsigned int> indexData;

	m_terrainPlane.m_indexCount = size;

	int j = 0;
	for (unsigned int i = 0; i < size; i += 6)
	{
		if ((j + 1) % width)
		{

			indexData.push_back(j);
			indexData.push_back(j + 1);
			indexData.push_back(j + height + 1);

			indexData.push_back(j);
			indexData.push_back(j + height + 1);
			indexData.push_back(j + height);
		}
		else
			i -= 6;
		j++;
	}


	glGenVertexArrays(1, &m_terrainPlane.m_VAO);
	glBindVertexArray(m_terrainPlane.m_VAO);

	glGenBuffers(1, &m_terrainPlane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainPlane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* vertexData.size(),
		&vertexData.front(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_terrainPlane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainPlane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indexData.size(),
		&indexData.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), ((char*)0) + sizeof(float) * 4);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), ((char*)0) + sizeof(float) * 6);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::CreateWaterPlane(int dims)
{
	std::vector<Vertex> vertexData;

	for (int i = 0; i < dims; ++i)
	{
		for (int j = 0; j < dims; ++j)
		{
			Vertex point;
			point.v_x = (float)(-(dims * 100) / 2) + (i * 100);
			point.v_y = 0;
			point.v_z = (float)(-(dims * 100) / 2) + (j * 100);
			point.v_w = 1;
			point.t_x = 1.0f / dims * i;
			point.t_y = 1.0f / dims * j;

			vertexData.push_back(point);
		}
	}

	unsigned int size = (((dims - 1) * 6) * dims) - (6 * (dims - 1));
	//unsigned int indexData[size];
	std::vector<unsigned int> indexData;

	m_waterPlane.m_indexCount = size;

	int j = 0;
	for (unsigned int i = 0; i < size; i += 6)
	{
		if ((j + 1) % dims)
		{

			indexData.push_back(j);
			indexData.push_back(j + 1);
			indexData.push_back(j + dims + 1);

			indexData.push_back(j);
			indexData.push_back(j + dims + 1);
			indexData.push_back(j + dims);
		}
		else
			i -= 6;
		j++;
	}

	glGenVertexArrays(1, &m_waterPlane.m_VAO);
	glBindVertexArray(m_waterPlane.m_VAO);

	glGenBuffers(1, &m_waterPlane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_waterPlane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* vertexData.size(),
		&vertexData.front(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_waterPlane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_waterPlane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indexData.size(),
		&indexData.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), ((char*)0) + sizeof(float) * 4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::CreatePerlin(int dims)
{
	float *perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * 3;
	int octaves = 6;

	for (int x = 0; x < dims; ++x)
	{
		for (int y = 0; y < dims; ++y)
		{
			float amplitude = 1.0f;
			float persistence = 0.3f;
			perlin_data[y * dims + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dims, dims, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::CreateDiamondSquare(int dims)
{
	m_terrain = new vec3 *[dims];
	float **diamond_square = new float*[dims];
	
	for (int i = 0; i < dims; ++i)
	{
		diamond_square[i] = new float[dims];
		m_terrain[i] = new vec3[dims];
	}
	
	diamond_square[0][0] = (float)(rand() % 100) / 100.f * ((dims * 0.7) * 2 - (dims * 0.7));
	m_terrain[0][0].y = diamond_square[0][0];
	diamond_square[dims - 1][0] = (float)(rand() % 100) / 100.f * ((dims * 0.7) * 2 - (dims * 0.7));
	m_terrain[dims - 1][0].y = diamond_square[dims - 1][0];
	diamond_square[0][dims - 1] = (float)(rand() % 100) / 100.f * ((dims * 0.7) * 2 - (dims * 0.7));
	m_terrain[0][dims - 1].y = diamond_square[0][dims - 1];
	diamond_square[dims - 1][dims - 1] = (float)(rand() % 100) / 100.f * ((dims * 0.7) * 2 - (dims * 0.7));
	m_terrain[dims - 1][dims - 1].y = diamond_square[dims - 1][dims - 1];
	
	int sideLength = dims - 1;
	float scale;
	//unsigned int squares = 1;
	
	while (sideLength > 1)
	{
		scale = (float)sideLength * 0.7f;
	
		for (int i = sideLength / 2; i < dims - 1; i += sideLength)
		{
			for (int j = sideLength / 2; j < dims - 1; j += sideLength)
			{
				float average =
					(diamond_square[i - sideLength / 2][j - sideLength / 2] +
					diamond_square[i - sideLength / 2][j + sideLength / 2] +
					diamond_square[i + sideLength / 2][j - sideLength / 2] +
					diamond_square[i + sideLength / 2][j + sideLength / 2]) / 4;

				average += ((float)(rand() % 100) / 100.f) * scale * 2 - scale;
				diamond_square[i][j] = average;
				m_terrain[i][j].y = average;
			}
		}

		//then diamonds
		for (int i = 0; i < (dims); i += sideLength / 2)
		{
			for (int j = (i + sideLength / 2) % sideLength; j <= dims - 1; j += sideLength)
			{
				float average = 0; 
				float count = 0;
				if ((i >= 0 && i < dims) && 
					(j - (sideLength / 2) >= 0 && j - (sideLength / 2) < dims))
				{
					average += diamond_square[i][j - (sideLength / 2)];
					count++;
				}
				if (i + (sideLength / 2) >= 0 && i + (sideLength / 2) < dims &&
					j >= 0 && j < dims)
				{
					average += diamond_square[i + (sideLength / 2)][j];
					count++;
				}
				if ((i >= 0 && i < dims) && 
					j + (sideLength / 2) >= 0 && j + (sideLength / 2) < dims)
				{
					average += diamond_square[i][j + (sideLength / 2)];
					count++;
				}
				if (i - (sideLength / 2) >= 0 && i - (sideLength / 2) < dims &&
					j >= 0 && j < dims)
				{
					average += diamond_square[i - (sideLength / 2)][j];
					count++;
				}
				average /= count;
	
				average += ((float)(rand() % 100) / 100.f) * scale * 2 - scale;
				diamond_square[i][j] = average;
				m_terrain[i][j].y = average;
			}
		}

		//squares = (sqrt(squares) * 2) * 2;
		sideLength /= 2;
	}

	//smooth points
	int count = 0;
	int total = 0;

	for (int repetition = 0; repetition < 0; ++repetition)
	{
		for (int x = 0; x < dims - 1; ++x)
		{
			for (int y = 0; y < dims - 1; ++y)
			{
				count = 0;
				total = 0;

				for (int i = x - 3; i <= x + 3; ++i)
				{
					if (i < 0 || i > dims - 1)
						continue;
					for (int j = y - 3; j <= y + 3; ++j)
					{
						if (j < 0 || j > dims - 1)
							continue;

						total += diamond_square[i][j];
						count++;
					}
				}
				if (count != 0 && total != 0)
				{
					diamond_square[x][y] = total / (float)count;
					m_terrain[x][y].y = diamond_square[x][y];
				}
			}
		}
	}

	float *data = new float[dims * dims];

	for (int x = 0; x < dims; ++x)
	{
		for (int y = 0; y < dims; ++y)
		{
			data[(x * dims) + y] = diamond_square[x][y];
		}

	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dims, dims, 0, GL_RED, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::Update(float timer, float dt, mat4 *cameraTransform)
{
	for (auto model : m_models)
	{
		if (m_animateFBX && model->m_fbx->getSkeletonCount() > 0)
		{
			FBXSkeleton* skeleton = model->m_fbx->getSkeletonByIndex(0);
			FBXAnimation* animation = model->m_fbx->getAnimationByIndex(0);

			skeleton->evaluate(animation, timer);

			for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
			{
				skeleton->m_nodes[bone_index]->updateGlobalTransform();
			}
		}
	}
	//for (int i = 0; i < 10; ++i)
	//{
	//	if (m_models[i].m_fbx != NULL)
	//	{
	//		if (m_animateFBX && m_models[i].m_fbx->getSkeletonCount() > 0)
	//		{
	//			FBXSkeleton* skeleton = m_models[i].m_fbx->getSkeletonByIndex(0);
	//			FBXAnimation* animation = m_models[i].m_fbx->getAnimationByIndex(0);
	//
	//			skeleton->evaluate(animation, timer);
	//
	//			for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	//			{
	//				skeleton->m_nodes[bone_index]->updateGlobalTransform();
	//			}
	//		}
	//	}
	//}

	if (m_emitter != NULL)
	{
		m_emitter->Update(dt, *cameraTransform);
	}
}

void Renderer::Draw(vec3 *light, vec3* lightColour, mat4 *lightMatrix,
	mat4* projectionView, vec3* cameraPos, float* specPow, float* height, float* waterHeight, float* time)
{
	unsigned int loc;
	unsigned int modelCount = 10;

	mat4 emptyTransform(1.f);

	if (m_sbo != NULL)
	{
		//render shadow map
		mat4 lightProj = glm::ortho<float>(-5000, 5000, -5000, 5000, -5000, 5000);
		mat4 lightView = glm::lookAt(*light, vec3(0), vec3(0, 1, 0));
		*lightMatrix = lightProj * lightView;

		// shadow pass: bind our shadow map target and clear the depth
		glBindFramebuffer(GL_FRAMEBUFFER, m_sbo);
		glViewport(0, 0, 4096, 4096);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_shadowGenProgram);

		// bind the light matrix
		loc = glGetUniformLocation(m_shadowGenProgram, "LightMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*lightMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
		loc = glGetUniformLocation(m_shadowGenProgram, "perlin_texture");
		glUniform1i(loc, 0);

		loc = glGetUniformLocation(m_shadowGenProgram, "height");
		glUniform1f(loc, *height);

		// draw all shadow-casting geometry
		if (m_glInfo.size() != 0)
		{
			loc = glGetUniformLocation(m_shadowGenProgram, "texcoord");
			glUniform2f(loc, 0, 0);

			loc = glGetUniformLocation(m_shadowGenProgram, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(emptyTransform));

			for (unsigned int i = 0; i < m_glInfo.size(); ++i)
			{
				glBindVertexArray(m_glInfo[i].m_VAO);
				glDrawElements(GL_TRIANGLES, m_glInfo[i].m_indexCount, GL_UNSIGNED_INT, 0);
			}
		}

		for (auto model : m_models)
		{
			mat4 transform = model->m_scale * model->m_rotation * model->m_position;

			loc = glGetUniformLocation(m_shadowGenProgram, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

			loc = glGetUniformLocation(m_shadowGenProgram, "texcoord");
			glUniform2f(loc, model->m_heightTexCoord.x, model->m_heightTexCoord.y);

			if (model->m_fbx->getSkeletonCount() == 0 || !model->m_animate)
			{
				for (unsigned int j = 0; j < model->m_fbx->getMeshCount(); ++j)
				{
					FBXMeshNode* mesh = model->m_fbx->getMeshByIndex(j);

					unsigned int* glData = (unsigned int*)mesh->m_userData;

					glBindVertexArray(glData[0]);
					glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}

		//for (int i = 0; i < modelCount; ++i)
		//{
		//	if (m_models[i].m_fbx != NULL)
		//	{
		//		mat4 transform = m_models[i].m_scale * m_models[i].m_rotation * m_models[i].m_position;
		//
		//		loc = glGetUniformLocation(m_shadowGenProgram, "Transform");
		//		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		//
		//		loc = glGetUniformLocation(m_shadowGenProgram, "texcoord");
		//		glUniform2f(loc, m_models[i].m_heightTexCoord.x, m_models[i].m_heightTexCoord.y);
		//
		//		if (m_models[i].m_fbx->getSkeletonCount() == 0 || !m_models[i].m_animate)
		//		{
		//			for (unsigned int j = 0; j < m_models[i].m_fbx->getMeshCount(); ++j)
		//			{
		//				FBXMeshNode* mesh = m_models[i].m_fbx->getMeshByIndex(j);
		//
		//				unsigned int* glData = (unsigned int*)mesh->m_userData;
		//
		//				glBindVertexArray(glData[0]);
		//				glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		//			}
		//		}
		//	}
		//}

		glUseProgram(m_shadowGenProgramAnim);

		loc = glGetUniformLocation(m_shadowGenProgramAnim, "LightMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*lightMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
		loc = glGetUniformLocation(m_shadowGenProgramAnim, "perlin_texture");
		glUniform1i(loc, 0);

		loc = glGetUniformLocation(m_shadowGenProgramAnim, "height");
		glUniform1f(loc, *height);

		for (auto model : m_models)
		{
			mat4 transform = model->m_rotation * model->m_position * model->m_scale;

			loc = glGetUniformLocation(m_shadowGenProgramAnim, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

			loc = glGetUniformLocation(m_shadowGenProgramAnim, "texcoord");
			glUniform2f(loc, model->m_heightTexCoord.x, model->m_heightTexCoord.y);

			if (model->m_fbx->getSkeletonCount() > 0 && model->m_animate)
			{
				FBXSkeleton* skeleton = model->m_fbx->getSkeletonByIndex(0);
				skeleton->updateBones();

				loc = glGetUniformLocation(m_shadowGenProgramAnim, "bones");
				glUniformMatrix4fv(loc, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

				for (unsigned int j = 0; j < model->m_fbx->getMeshCount(); ++j)
				{
					FBXMeshNode* mesh = model->m_fbx->getMeshByIndex(j);

					unsigned int* glData = (unsigned int*)mesh->m_userData;

					glBindVertexArray(glData[0]);
					glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}

		//for (int i = 0; i < modelCount; ++i)
		//{
		//	if (m_models[i].m_fbx != NULL)
		//	{
		//		mat4 transform = m_models[i].m_rotation * m_models[i].m_position * m_models[i].m_scale;
		//
		//		loc = glGetUniformLocation(m_shadowGenProgramAnim, "Transform");
		//		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		//
		//		loc = glGetUniformLocation(m_shadowGenProgramAnim, "texcoord");
		//		glUniform2f(loc, m_models[i].m_heightTexCoord.x, m_models[i].m_heightTexCoord.y);
		//
		//		if (m_models[i].m_fbx->getSkeletonCount() > 0 && m_models[i].m_animate)
		//		{
		//			FBXSkeleton* skeleton = m_models[i].m_fbx->getSkeletonByIndex(0);
		//			skeleton->updateBones();
		//
		//			loc = glGetUniformLocation(m_shadowGenProgramAnim, "bones");
		//			glUniformMatrix4fv(loc, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
		//
		//			for (unsigned int j = 0; j < m_models[i].m_fbx->getMeshCount(); ++j)
		//			{
		//				FBXMeshNode* mesh = m_models[i].m_fbx->getMeshByIndex(j);
		//
		//				unsigned int* glData = (unsigned int*)mesh->m_userData;
		//
		//				glBindVertexArray(glData[0]);
		//				glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		//			}
		//		}
		//	}
		//}

		BindFrameBuffer(true);

		//draw shadow map
		glUseProgram(m_shadowProgramAnim);

		mat4 textureSpaceOffset(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
			);

		mat4 newLightMatrix = textureSpaceOffset * (*lightMatrix);

		for (auto model : m_models)
		{
			if (model->m_fbx->getSkeletonCount() > 0 && model->m_animate)
			{
				FBXSkeleton* skeleton = model->m_fbx->getSkeletonByIndex(0);
				skeleton->updateBones();

				loc = glGetUniformLocation(m_shadowProgramAnim, "ProjectionView");
				glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

				loc = glGetUniformLocation(m_shadowProgramAnim, "LightMatrix");
				glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(newLightMatrix));

				mat4 transform = model->m_rotation * model->m_position * model->m_scale;

				loc = glGetUniformLocation(m_shadowProgramAnim, "Transform");
				glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

				loc = glGetUniformLocation(m_shadowProgramAnim, "light");
				glUniform3fv(loc, 1, glm::value_ptr(*light));

				loc = glGetUniformLocation(m_shadowProgramAnim, "cameraPos");
				glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));

				loc = glGetUniformLocation(m_shadowProgramAnim, "lightColor");
				glUniform3fv(loc, 1, glm::value_ptr(*lightColour));

				loc = glGetUniformLocation(m_shadowProgramAnim, "specPow");
				glUniform1fv(loc, 1, specPow);

				loc = glGetUniformLocation(m_shadowProgramAnim, "shadowBias");
				glUniform1f(loc, 0.01f);

				loc = glGetUniformLocation(m_shadowProgramAnim, "bones");
				glUniformMatrix4fv(loc, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, model->m_texture);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_normal);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, m_sboDepth);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

				loc = glGetUniformLocation(m_shadowProgramAnim, "diffuse");
				glUniform1i(loc, 0);

				loc = glGetUniformLocation(m_shadowProgramAnim, "normal");
				glUniform1i(loc, 1);

				loc = glGetUniformLocation(m_shadowProgramAnim, "shadowMap");
				glUniform1i(loc, 2);

				loc = glGetUniformLocation(m_shadowProgramAnim, "perlin_texture");
				glUniform1i(loc, 3);

				loc = glGetUniformLocation(m_shadowProgramAnim, "height");
				glUniform1f(loc, *height);

				loc = glGetUniformLocation(m_shadowProgramAnim, "texcoord");
				glUniform2f(loc, model->m_heightTexCoord.x, model->m_heightTexCoord.y);

				for (unsigned int j = 0; j < model->m_fbx->getMeshCount(); ++j)
				{
					FBXMeshNode* mesh = model->m_fbx->getMeshByIndex(j);

					unsigned int* glData = (unsigned int*)mesh->m_userData;

					glBindVertexArray(glData[0]);
					glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}

		//for (int i = 0; i < modelCount; ++i)
		//{
		//	if (m_models[i].m_fbx != NULL)
		//	{
		//		if (m_models[i].m_fbx->getSkeletonCount() > 0 && m_models[i].m_animate)
		//		{
		//			FBXSkeleton* skeleton = m_models[i].m_fbx->getSkeletonByIndex(0);
		//			skeleton->updateBones();
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "ProjectionView");
		//			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "LightMatrix");
		//			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(newLightMatrix));
		//
		//			mat4 transform = m_models[i].m_rotation * m_models[i].m_position * m_models[i].m_scale;
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "Transform");
		//			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "light");
		//			glUniform3fv(loc, 1, glm::value_ptr(*light));
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "cameraPos");
		//			glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "lightColor");
		//			glUniform3fv(loc, 1, glm::value_ptr(*lightColour));
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "specPow");
		//			glUniform1fv(loc, 1, specPow);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "shadowBias");
		//			glUniform1f(loc, 0.01f);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "bones");
		//			glUniformMatrix4fv(loc, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
		//
		//			glActiveTexture(GL_TEXTURE0);
		//			glBindTexture(GL_TEXTURE_2D, m_models[i].m_texture);
		//
		//			glActiveTexture(GL_TEXTURE1);
		//			glBindTexture(GL_TEXTURE_2D, m_normal);
		//
		//			glActiveTexture(GL_TEXTURE2);
		//			glBindTexture(GL_TEXTURE_2D, m_sboDepth);
		//
		//			glActiveTexture(GL_TEXTURE3);
		//			glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "diffuse");
		//			glUniform1i(loc, 0);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "normal");
		//			glUniform1i(loc, 1);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "shadowMap");
		//			glUniform1i(loc, 2);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "perlin_texture");
		//			glUniform1i(loc, 3);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "height");
		//			glUniform1f(loc, *height);
		//
		//			loc = glGetUniformLocation(m_shadowProgramAnim, "texcoord");
		//			glUniform2f(loc, m_models[i].m_heightTexCoord.x, m_models[i].m_heightTexCoord.y);
		//
		//			for (unsigned int j = 0; j < m_models[i].m_fbx->getMeshCount(); ++j)
		//			{
		//				FBXMeshNode* mesh = m_models[i].m_fbx->getMeshByIndex(j);
		//
		//				unsigned int* glData = (unsigned int*)mesh->m_userData;
		//
		//				glBindVertexArray(glData[0]);
		//				glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		//			}
		//		}
		//	}
		//}

		glUseProgram(m_shadowProgram);

		//z rotation
		//mat4 rotation{ cosf(90), -sinf(90), 0.f, 0.f,
		//			   sinf(90),  cosf(90), 0.f, 0.f,
		//					0.f,	   0.f, 1.f, 0.f,
		//					0.f,	   0.f, 0.f, 1.f};

		//y rotation
		//mat4 rotation{ cosf(90), 0.f,  sinf(90), 0.f,
		//	0.f, 1.f, 0.f, 0.f,
		//	-sinf(90), 0.f, cosf(90), 0.f,
		//	0.f, 0.f, 0.f, 1.f };

		//x rotation

		loc = glGetUniformLocation(m_shadowProgram, "ProjectionView");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

		loc = glGetUniformLocation(m_shadowProgram, "LightMatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(newLightMatrix));

		loc = glGetUniformLocation(m_shadowProgram, "light");
		glUniform3fv(loc, 1, glm::value_ptr(*light));

		loc = glGetUniformLocation(m_shadowProgram, "cameraPos");
		glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));

		loc = glGetUniformLocation(m_shadowProgram, "lightColor");
		glUniform3fv(loc, 1, glm::value_ptr(*lightColour));

		loc = glGetUniformLocation(m_shadowProgram, "specPow");
		glUniform1fv(loc, 1, specPow);

		loc = glGetUniformLocation(m_shadowProgram, "shadowBias");
		glUniform1f(loc, 0.01f);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normal);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_sboDepth);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

		loc = glGetUniformLocation(m_shadowProgram, "normal");
		glUniform1i(loc, 1);

		loc = glGetUniformLocation(m_shadowProgram, "shadowMap");
		glUniform1i(loc, 3);

		loc = glGetUniformLocation(m_shadowProgram, "perlin_texture");
		glUniform1i(loc, 2);

		loc = glGetUniformLocation(m_shadowProgram, "height");
		glUniform1f(loc, *height);

		if (m_glInfo.size() != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture);

			loc = glGetUniformLocation(m_shadowProgram, "diffuse");
			glUniform1i(loc, 0);

			loc = glGetUniformLocation(m_shadowProgram, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(emptyTransform));

			loc = glGetUniformLocation(m_shadowProgram, "texcoord");
			glUniform2f(loc, 0, 0);

			for (unsigned int i = 0; i < m_glInfo.size(); ++i)
			{
				glBindVertexArray(m_glInfo[i].m_VAO);
				glDrawElements(GL_TRIANGLES, m_glInfo[i].m_indexCount, GL_UNSIGNED_INT, 0);
			}
		}

		for (auto model : m_models)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model->m_texture);

			loc = glGetUniformLocation(m_shadowProgram, "diffuse");
			glUniform1i(loc, 0);

			mat4 transform = model->m_rotation * model->m_position * model->m_scale;

			loc = glGetUniformLocation(m_shadowProgram, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

			loc = glGetUniformLocation(m_shadowProgram, "texcoord");
			glUniform2f(loc, model->m_heightTexCoord.x, model->m_heightTexCoord.y);

			if (model->m_fbx->getSkeletonCount() == 0 || !model->m_animate)
			{
				for (unsigned int j = 0; j < model->m_fbx->getMeshCount(); ++j)
				{
					FBXMeshNode* mesh = model->m_fbx->getMeshByIndex(j);

					unsigned int* glData = (unsigned int*)mesh->m_userData;

					glBindVertexArray(glData[0]);
					glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}

		//for (int i = 0; i < modelCount; i++)
		//{
		//	if (m_models[i].m_fbx != NULL)
		//	{
		//		glActiveTexture(GL_TEXTURE0);
		//		glBindTexture(GL_TEXTURE_2D, m_models[i].m_texture);
		//
		//		loc = glGetUniformLocation(m_shadowProgram, "diffuse");
		//		glUniform1i(loc, 0);
		//
		//		mat4 transform = m_models[i].m_rotation * m_models[i].m_position * m_models[i].m_scale;
		//
		//		loc = glGetUniformLocation(m_shadowProgram, "Transform");
		//		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		//
		//		loc = glGetUniformLocation(m_shadowProgram, "texcoord");
		//		glUniform2f(loc, m_models[i].m_heightTexCoord.x, m_models[i].m_heightTexCoord.y);
		//
		//		if (m_models[i].m_fbx->getSkeletonCount() == 0 || !m_models[i].m_animate)
		//		{
		//			for (unsigned int j = 0; j < m_models[i].m_fbx->getMeshCount(); ++j)
		//			{
		//				FBXMeshNode* mesh = m_models[i].m_fbx->getMeshByIndex(j);
		//
		//				unsigned int* glData = (unsigned int*)mesh->m_userData;
		//
		//				glBindVertexArray(glData[0]);
		//				glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		//			}
		//		}
		//	}
		//}

		if (m_terrainPlane.m_indexCount != NULL)// needs indexcount to be set
		{
			glUseProgram(m_terrainGenShadowProgram);

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "view_proj");
			glUniformMatrix4fv(loc, 1, GL_FALSE,
				glm::value_ptr(*projectionView));

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "LightMatrix");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(newLightMatrix));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
			loc = glGetUniformLocation(m_terrainGenShadowProgram, "perlin_texture");
			glUniform1i(loc, 0);

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "height");
			glUniform1f(loc, *height);

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "waterHeight");
			glUniform1f(loc, *waterHeight);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_grass_texture);
			loc = glGetUniformLocation(m_terrainGenShadowProgram, "grass_texture");
			glUniform1i(loc, 1);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, m_snow_texture);
			loc = glGetUniformLocation(m_terrainGenShadowProgram, "snow_texture");
			glUniform1i(loc, 4);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, m_sand_texture);
			loc = glGetUniformLocation(m_terrainGenShadowProgram, "sand_texture");
			glUniform1i(loc, 5);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, m_rock_texture);
			loc = glGetUniformLocation(m_terrainGenShadowProgram, "rock_texture");
			glUniform1i(loc, 3);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_sboDepth);
			loc = glGetUniformLocation(m_terrainGenShadowProgram, "shadowMap");
			glUniform1i(loc, 2);

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "shadowBias");
			glUniform1f(loc, 0.01f);

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "light");
			glUniform3fv(loc, 1, glm::value_ptr(*light));

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "cameraPos");
			glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "lightColor");
			glUniform3fv(loc, 1, glm::value_ptr(*lightColour));

			loc = glGetUniformLocation(m_terrainGenShadowProgram, "specPow");
			glUniform1fv(loc, 1, specPow);

			glBindVertexArray(m_terrainPlane.m_VAO);
			glDrawElements(GL_TRIANGLES, m_terrainPlane.m_indexCount, GL_UNSIGNED_INT, nullptr);
		}

		//glBindVertexArray(m_shadowPlane.m_VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		BindFrameBuffer(true);

		if (m_glInfo.size() != 0)
		{
			glUseProgram(m_programID);

			loc = glGetUniformLocation(m_programID, "ProjectionView");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

			loc = glGetUniformLocation(m_programID, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(emptyTransform));

			loc = glGetUniformLocation(m_programID, "light");
			glUniform3fv(loc, 1, glm::value_ptr(*light));

			loc = glGetUniformLocation(m_programID, "cameraPos");
			glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));

			loc = glGetUniformLocation(m_programID, "lightColor");
			glUniform3fv(loc, 1, glm::value_ptr(*lightColour));

			loc = glGetUniformLocation(m_programID, "specPow");
			glUniform1fv(loc, 1, specPow);

			loc = glGetUniformLocation(m_programID, "diffuse");
			glUniform1i(loc, 0);

			//set texture slot
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture);

			for (unsigned int i = 0; i < m_glInfo.size(); ++i)
			{
				glBindVertexArray(m_glInfo[i].m_VAO);
				glDrawElements(GL_TRIANGLES, m_glInfo[i].m_indexCount, GL_UNSIGNED_INT, 0);
			}
		}

		for (auto model : m_models)
		{
			FBXSkeleton* skeleton = model->m_fbx->getSkeletonByIndex(0);
			skeleton->updateBones();

			glUseProgram(m_programID);

			loc = glGetUniformLocation(m_programID, "ProjectionView");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

			mat4 transform = model->m_rotation * model->m_position * model->m_scale;

			loc = glGetUniformLocation(m_programID, "Transform");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

			loc = glGetUniformLocation(m_programID, "bones");
			glUniformMatrix4fv(loc, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

			loc = glGetUniformLocation(m_programID, "light");
			glUniform3fv(loc, 1, glm::value_ptr(*light));

			loc = glGetUniformLocation(m_programID, "cameraPos");
			glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));

			loc = glGetUniformLocation(m_programID, "lightColor");
			glUniform3fv(loc, 1, glm::value_ptr(*lightColour));

			loc = glGetUniformLocation(m_programID, "specPow");
			glUniform1fv(loc, 1, specPow);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model->m_texture);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_normal);

			loc = glGetUniformLocation(m_programID, "diffuse");
			glUniform1i(loc, 0);

			loc = glGetUniformLocation(m_programID, "normal");
			glUniform1i(loc, 1);

			for (unsigned int j = 0; j < model->m_fbx->getMeshCount(); ++j)
			{
				FBXMeshNode* mesh = model->m_fbx->getMeshByIndex(j);

				unsigned int* glData = (unsigned int*)mesh->m_userData;

				glBindVertexArray(glData[0]);
				glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
			}
		}

		//for (int i = 0; i < modelCount; ++i)
		//{
		//	if (m_models[i].m_fbx != NULL)
		//	{
		//		FBXSkeleton* skeleton = m_models[i].m_fbx->getSkeletonByIndex(0);
		//		skeleton->updateBones();
		//
		//		glUseProgram(m_programID);
		//
		//		loc = glGetUniformLocation(m_programID, "ProjectionView");
		//		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));
		//
		//		mat4 transform = m_models[i].m_rotation * m_models[i].m_position * m_models[i].m_scale;
		//
		//		loc = glGetUniformLocation(m_programID, "Transform");
		//		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));
		//
		//		loc = glGetUniformLocation(m_programID, "bones");
		//		glUniformMatrix4fv(loc, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
		//
		//		loc = glGetUniformLocation(m_programID, "light");
		//		glUniform3fv(loc, 1, glm::value_ptr(*light));
		//
		//		loc = glGetUniformLocation(m_programID, "cameraPos");
		//		glUniform3fv(loc, 1, glm::value_ptr(*cameraPos));
		//
		//		loc = glGetUniformLocation(m_programID, "lightColor");
		//		glUniform3fv(loc, 1, glm::value_ptr(*lightColour));
		//
		//		loc = glGetUniformLocation(m_programID, "specPow");
		//		glUniform1fv(loc, 1, specPow);
		//
		//		glActiveTexture(GL_TEXTURE0);
		//		glBindTexture(GL_TEXTURE_2D, m_models[i].m_texture);
		//
		//		glActiveTexture(GL_TEXTURE1);
		//		glBindTexture(GL_TEXTURE_2D, m_normal);
		//
		//		loc = glGetUniformLocation(m_programID, "diffuse");
		//		glUniform1i(loc, 0);
		//
		//		loc = glGetUniformLocation(m_programID, "normal");
		//		glUniform1i(loc, 1);
		//
		//		for (unsigned int j = 0; j < m_models[i].m_fbx->getMeshCount(); ++j)
		//		{
		//			FBXMeshNode* mesh = m_models[i].m_fbx->getMeshByIndex(j);
		//
		//			unsigned int* glData = (unsigned int*)mesh->m_userData;
		//
		//			glBindVertexArray(glData[0]);
		//			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		//		}
		//	}
		//}

		if (m_terrainPlane.m_indexCount != NULL)// needs indexcount to be set
		{
			glUseProgram(m_terrainGenProgram);

			loc = glGetUniformLocation(m_terrainGenProgram, "view_proj");
			glUniformMatrix4fv(loc, 1, GL_FALSE,
				glm::value_ptr(*projectionView));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
			loc = glGetUniformLocation(m_terrainGenProgram, "perlin_texture");
			glUniform1i(loc, 0);

			loc = glGetUniformLocation(m_terrainGenProgram, "height");
			glUniform1f(loc, *height);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_rock_texture);
			loc = glGetUniformLocation(m_terrainGenProgram, "rock_texture");
			glUniform1i(loc, 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_grass_texture);
			loc = glGetUniformLocation(m_terrainGenProgram, "grass_texture");
			glUniform1i(loc, 2);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, m_snow_texture);
			loc = glGetUniformLocation(m_terrainGenProgram, "snow_texture");
			glUniform1i(loc, 3);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, m_sand_texture);
			loc = glGetUniformLocation(m_terrainGenProgram, "sand_texture");
			glUniform1i(loc, 4);

			glBindVertexArray(m_terrainPlane.m_VAO);
			glDrawElements(GL_TRIANGLES, m_terrainPlane.m_indexCount, GL_UNSIGNED_INT, nullptr);
		}

	}

	if (m_emitter != NULL)
	{
		glUseProgram(m_programID);
		loc = glGetUniformLocation(m_programID, "ProjectionView");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

		m_emitter->Draw();
	}

	if (m_waterPlane.m_indexCount != NULL)// needs indexcount to be set
	{
		glUseProgram(m_waterProgram);

		loc = glGetUniformLocation(m_waterProgram, "view_proj");
		glUniformMatrix4fv(loc, 1, GL_FALSE,
			glm::value_ptr(*projectionView));

		loc = glGetUniformLocation(m_waterProgram, "waterHeight");
		glUniform1f(loc, *waterHeight);

		loc = glGetUniformLocation(m_waterProgram, "time");
		glUniform1f(loc, *time);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_water_texture);
		loc = glGetUniformLocation(m_waterProgram, "water_texture");
		glUniform1i(loc, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_water_height_1);
		loc = glGetUniformLocation(m_waterProgram, "water_height_1");
		glUniform1i(loc, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_water_height_2);
		loc = glGetUniformLocation(m_waterProgram, "water_height_2");
		glUniform1i(loc, 2);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(m_waterPlane.m_VAO);
		glDrawElements(GL_TRIANGLES, m_waterPlane.m_indexCount, GL_UNSIGNED_INT, nullptr);

		glDisable(GL_BLEND);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

	BindFrameBuffer(false);

	//must be done last
	if (m_viewPlane.m_indexCount != NULL)//needs indexcount to be set
	{
		glUseProgram(m_postProcessProgram);

		loc = glGetUniformLocation(m_postProcessProgram, "ProjectionView");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_fboTexture);
		glUniform1i(glGetUniformLocation(m_postProcessProgram, "diffuse"), 0);

		glBindVertexArray(m_viewPlane.m_VAO);
		glDrawElements(GL_TRIANGLES, m_viewPlane.m_indexCount, GL_UNSIGNED_INT, nullptr);
	}

}

void Renderer::FillModel(int i, mat4 pos, mat4 rot, mat4 scale)
{
	m_models[i]->m_position = pos;
	m_models[i]->m_rotation = rot;
	m_models[i]->m_scale = scale;
}

void Renderer::SetModelTexture(int i, std::string name)
{
	m_models[i]->m_texture = *m_textures[name];
}

vec3 Renderer::GetTerrainPos(int x, int y)
{
	return m_terrain[x][y];
}

vec2 Renderer::GetTerrainTexCoord(int x, int y)
{
	return m_terrainTexCoords[x][y];
}

void Renderer::SetModelPos(int i, vec3 pos)
{
	m_models[i]->m_position = glm::translate(pos);
}

void Renderer::SetModelHeightTexCoord(int i, glm::vec2 texCoord)
{
	m_models[i]->m_heightTexCoord = texCoord;
}

void Renderer::SetModelScale(int i, float scale)
{
	m_models[i]->m_scale = glm::scale(vec3(scale));
}
