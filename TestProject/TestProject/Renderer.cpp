#include "Renderer.h"
#include <stb_image.h>
#include <fstream>

using glm::vec3;

Renderer::Renderer()
{
	m_objects = 0;
	m_animateFBX = false;

	m_programs["m_programID"] = &m_programID;
	m_programs["m_postProcessProgram"] = &m_postProcessProgram;
	m_programs["m_shadowProgram"] = &m_shadowProgram;
	m_programs["m_shadowGenProgram"] = &m_shadowGenProgram;
	m_programs["m_perlinProgram"] = &m_perlinProgram;

	m_textures["m_texture"] = &m_texture;
	m_textures["m_snow_texture"] = &m_snow_texture;
	m_textures["m_grass_texture"] = &m_grass_texture;
	m_textures["m_rock_texture"] = &m_rock_texture;

}

Renderer::~Renderer()
{

}

void Renderer::Destroy()
{
	if (m_fbx != NULL)
		CleanupOpenGLBuffers(m_fbx);

	glDeleteProgram(m_programID);
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
	m_fbx = new FBXFile();
	m_fbx->load(path);
	CreateOpenGLBuffers(m_fbx);
}

void Renderer::UpdateFBX(float timer)
{
	if (m_animateFBX)
	{
		FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
		FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

		skeleton->evaluate(animation, timer);

		for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
		{
			skeleton->m_nodes[bone_index]->updateGlobalTransform();
		}
	}
}

void Renderer::LoadTexture(std::string texture, std::string path, GLint type)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(path.c_str(),
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, m_textures[texture]);
	glBindTexture(GL_TEXTURE_2D, *m_textures[texture]);
	glTexImage2D(GL_TEXTURE_2D, 0, type, imageWidth, imageHeight, 0, type, GL_UNSIGNED_BYTE, data);
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

		glGenVertexArrays(1, &m_glInfo[m_objects].m_VAO);
		glGenBuffers(1, &m_glInfo[m_objects].m_VBO);
		glGenBuffers(1, &m_glInfo[m_objects].m_IBO);
		glBindVertexArray(m_glInfo[m_objects].m_VAO);

		unsigned int float_count = shapes[m_objects].mesh.positions.size();
		float_count += shapes[m_objects].mesh.normals.size();
		float_count += shapes[m_objects].mesh.texcoords.size();

		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[m_objects].mesh.positions.begin(),
			shapes[m_objects].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
			shapes[m_objects].mesh.normals.begin(),
			shapes[m_objects].mesh.normals.end());

		for (unsigned int j = 1; j < shapes[m_objects].mesh.texcoords.size(); j += 2)
		{
			shapes[m_objects].mesh.texcoords[j] = -shapes[m_objects].mesh.texcoords[j];
		}

		vertex_data.insert(vertex_data.end(),
			shapes[m_objects].mesh.texcoords.begin(),
			shapes[m_objects].mesh.texcoords.end());

		m_glInfo[m_objects].m_indexCount =
			shapes[m_objects].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[m_objects].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glInfo[m_objects].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shapes[m_objects].mesh.indices.size() * sizeof(unsigned int),
			shapes[m_objects].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[m_objects].mesh.positions.size()));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0,
			(void*)((sizeof(float)*shapes[m_objects].mesh.positions.size()) + (sizeof(float)*shapes[m_objects].mesh.normals.size())));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Renderer::DrawOBJ(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow)
{
	glUseProgram(m_programID);

	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(*projectionView));

	int light_uniform = glGetUniformLocation(m_programID, "light");
	glUniform3fv(light_uniform, 1, glm::value_ptr(*light));

	int camera_uniform = glGetUniformLocation(m_programID, "cameraPos");
	glUniform3fv(camera_uniform, 1, glm::value_ptr(*cameraPos));

	unsigned int light_color_uniform = glGetUniformLocation(m_programID, "lightColor");
	glUniform3fv(light_color_uniform, 1, glm::value_ptr(*lightColour));

	unsigned int spec_pow_uniform = glGetUniformLocation(m_programID, "specPow");
	glUniform1fv(spec_pow_uniform, 1, specPow);

	unsigned int diffuse = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(diffuse, 0);

	//set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	for (unsigned int i = 0; i < m_glInfo.size(); ++i)
	{
		glBindVertexArray(m_glInfo[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_glInfo[i].m_indexCount, GL_UNSIGNED_INT, 0);
	}
}

void Renderer::DrawFBX(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow)
{
	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	skeleton->updateBones();

	glUseProgram(m_programID);

	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(*projectionView));

	int bones_location = glGetUniformLocation(m_programID, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	int light_uniform = glGetUniformLocation(m_programID, "light");
	glUniform3fv(light_uniform, 1, glm::value_ptr(*light));

	int camera_uniform = glGetUniformLocation(m_programID, "cameraPos");
	glUniform3fv(camera_uniform, 1, glm::value_ptr(*cameraPos));

	unsigned int light_color_uniform = glGetUniformLocation(m_programID, "lightColor");
	glUniform3fv(light_color_uniform, 1, glm::value_ptr(*lightColour));

	unsigned int spec_pow_uniform = glGetUniformLocation(m_programID, "specPow");
	glUniform1fv(spec_pow_uniform, 1, specPow);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal);

	unsigned int diffuse = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(diffuse, 0);

	unsigned int normal = glGetUniformLocation(m_programID, "normal");
	glUniform1i(normal, 1);

	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
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

void Renderer::CreateParticleEmitter()
{
	m_emitter = new ParticleEmitter();
	m_emitter->Init(1000, 500, 0.1f, 1.0f, 1, 5, 1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));
}

void Renderer::UpdateParticles(float dt, mat4 cameraTransform)
{
	m_emitter->Update(dt, cameraTransform);
}

void Renderer::DrawParticles(mat4 cameraProjectionView)
{
	glUseProgram(m_programID);
	int projection_view_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE, glm::value_ptr(cameraProjectionView));

	m_emitter->Draw();
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

void Renderer::RenderShadowMap(vec3 *light, mat4 *lightMatrix)
{
	mat4 lightProj = glm::ortho<float>(-5000, 5000, -5000, 5000, -5000, 5000);
	mat4 lightView = glm::lookAt(*light, vec3(0), vec3(0, 1, 0));
	*lightMatrix = lightProj * lightView;

	// shadow pass: bind our shadow map target and clear the depth
	glBindFramebuffer(GL_FRAMEBUFFER, m_sbo);
	glViewport(0, 0, 4096, 4096);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shadowGenProgram);

	// bind the light matrix
	int loc = glGetUniformLocation(m_shadowGenProgram, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*lightMatrix));

	// draw all shadow-casting geometry
	DrawFBXGeometry();
}

void Renderer::DrawShadowMap(mat4* projectionView, mat4 *lightMatrix, vec3 *light)
{
	glUseProgram(m_shadowProgram);

	int loc = glGetUniformLocation(m_shadowProgram, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

	mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	mat4 newLightMatrix = textureSpaceOffset * (*lightMatrix);

	loc = glGetUniformLocation(m_shadowProgram, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(newLightMatrix));

	loc = glGetUniformLocation(m_shadowProgram, "lightDir");
	glUniform3fv(loc, 1, glm::value_ptr(*light));

	loc = glGetUniformLocation(m_shadowProgram, "shadowMap");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(m_shadowProgram, "shadowBias");
	glUniform1f(loc, 0.01f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_sboDepth);

	//DrawFBXGeometry();

	glBindVertexArray(m_shadowPlane.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	/*
	//bind vertex array object and draw the mesh
	DrawFBXGeometry();

	glBindVertexArray(m_shadowPlane.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	*/
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

void Renderer::DrawViewPlane(mat4* projectionView, vec3* light, vec3* cameraPos, vec3* lightColour, float* specPow)
{
	glUseProgram(m_postProcessProgram);

	int loc = glGetUniformLocation(m_postProcessProgram, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(*projectionView));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glUniform1i(glGetUniformLocation(m_postProcessProgram, "diffuse"), 0);

	glBindVertexArray(m_viewPlane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_viewPlane.m_indexCount, GL_UNSIGNED_INT, nullptr);
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

void Renderer::DrawFBXGeometry()
{
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(),
			GL_UNSIGNED_INT, 0);
	}
}

void Renderer::CreatePlane()
{
	const int x = 100;
	const int z = 100;

	Vertex vertexData[x * z];

	for (int i = 0; i < x; ++i)
	{
		for (int j = 0; j < z; ++j)
		{
			vertexData[(x * i) + j].v_x = (float)(-(x * 10) / 2) + (i * 10);
			vertexData[(x * i) + j].v_y = 0;
			vertexData[(x * i) + j].v_z = (float)(-(z * 10) / 2) + (j * 10);
			vertexData[(x * i) + j].v_w = 1;
			vertexData[(x * i) + j].t_x = 1.0f / x * i;
			vertexData[(x * i) + j].t_y = 1.0f / z * j;
		}
	}

	const unsigned int size = (((x - 1) * 6) * z) - (6 * (x - 1));
	unsigned int indexData[size];

	m_plane.m_indexCount = size;

	int j = 0;
	for (int i = 0; i < size; i += 6)
	{
		if ((j + 1) % x)
		{
			indexData[i] = j;
			indexData[i + 1] = j + 1;
			indexData[i + 2] = j + z + 1;

			indexData[i + 3] = j;
			indexData[i + 4] = j + z + 1;
			indexData[i + 5] = j + z;
		}
		else
			i -= 6;
		j++;
	}


	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);

	glGenBuffers(1, &m_plane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* (x * z),
		vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_plane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* size,
		indexData, GL_STATIC_DRAW);

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

void Renderer::DrawPlane(mat4* projectionView)
{
	glUseProgram(m_perlinProgram);

	int loc = glGetUniformLocation(m_perlinProgram, "view_proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		glm::value_ptr(*projectionView));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	unsigned int diffuse = glGetUniformLocation(m_perlinProgram, "perlin_texture");
	glUniform1i(diffuse, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_rock_texture);
	unsigned int rock = glGetUniformLocation(m_perlinProgram, "rock_texture");
	glUniform1i(rock, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_grass_texture);
	unsigned int grass = glGetUniformLocation(m_perlinProgram, "grass_texture");
	glUniform1i(grass, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_snow_texture);
	unsigned int snow = glGetUniformLocation(m_perlinProgram, "snow_texture");
	glUniform1i(snow, 3);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_indexCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::CreatePerlin()
{
	int dims = 100;
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 100, 100, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}