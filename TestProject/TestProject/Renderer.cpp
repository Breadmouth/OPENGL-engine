#include "Renderer.h"
#include <stb_image.h>
#include <fstream>

using glm::vec3;

Renderer::Renderer()
{
	m_animateFBX = false;
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

void Renderer::LoadTexture(std::string path, GLint type)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(path.c_str(),
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
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
			printf("unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
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

bool Renderer::LoadShader(std::string vertex, std::string fragment)
{
	//Generate program
	m_programID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = LoadShaderFromFile(vertex, GL_VERTEX_SHADER);

	//Check for errors
	if (vertexShader == 0)
	{
		glDeleteProgram(m_programID);
		m_programID = 0;
		return false;
	}

	//Attach vertex shader to program
	glAttachShader(m_programID, vertexShader);


	//Create fragment shader
	GLuint fragmentShader = LoadShaderFromFile(fragment, GL_FRAGMENT_SHADER);

	//Check for errors
	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		glDeleteProgram(m_programID);
		m_programID = 0;
		return false;
	}

	//Attach fragment shader to program
	glAttachShader(m_programID, fragmentShader);

	//Link program
	glLinkProgram(m_programID);

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
	glUniform1i(normal, 0);

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