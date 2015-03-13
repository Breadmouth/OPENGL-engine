#include "Tutorial2.h"
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJLoader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial2::Tutorial2()
{
	//LoadObject("cube");
}

void Tutorial2::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	//GenerateGrid(500, 500);

	CreateShader();

	LoadObject("dragon");
}

void Tutorial2::Destroy()
{

}

void Tutorial2::Update(float dt)
{


	camera.Update(dt);
}

void Tutorial2::Draw()
{
	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	unsigned int heightScaleUniform = glGetUniformLocation(m_programID, "heightScale");
	glGetUniformLocation(m_programID, "ProjectionView");
	glGetUniformLocation(m_programID, "time");
	glGetUniformLocation(m_programID, "heightScale");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(camera.GetProjectionView()));
	glUniform1f(timeUniform, currentTime);
	glUniform1f(heightScaleUniform, 1);
	
	glBindVertexArray(m_VAO);
	glPolygonMode(GL_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	Gizmos::draw(camera.GetProjectionView());

}

void Tutorial2::GenerateGrid( unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);

			vec3 colour = vec3(sinf((c/(float)(cols - 1)) * (r/(float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1); 
		}
	}

	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];

	unsigned int index = 0;
	for ( unsigned int r = 0; r < (rows - 1); ++r)
	{
		for ( unsigned int c = 0; c < (cols - 1); ++c)
		{
			//first triangle
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			//second triangle
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	indexCount = index;

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] auiIndices;
	delete[] aoVertices;
}

void Tutorial2::CreateShader()
{
	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Colour; \
						   out vec4 vColour; \
						   uniform mat4 ProjectionView; \
						   uniform float time; \
						   uniform float heightScale; \
						   void main() \
						   { \
								vColour = Colour; \
								vec4 P = Position; \
								gl_Position = ProjectionView * P; \
						   }";

	const char* fsSource = "#version 410\n \
						   in vec4 vColour; \
						   out vec4 FragColor; \
						   void main() { FragColor = vColour; }";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void Tutorial2::LoadObject(char *path)
{
	std::vector<Vertex> vertex;

	objectLoader = new OBJLoader(path);

	for (int i = 0; i < objectLoader->GetModel()->vertices.size(); ++i)
	{
		Vertex vert;
		vert.position = vec4(objectLoader->GetModel()->vertices[i], 1);
		vert.colour = vec4(objectLoader->GetModel()->colors[i],1);

		vertex.push_back(vert);
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	
	glGenVertexArrays(1, &m_VAO);
	
	glBindVertexArray(m_VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) * 2));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objectLoader->GetModel()->vIndices.size() * sizeof(GLuint), &objectLoader->GetModel()->vIndices[0], GL_STATIC_DRAW);
	indexCount = objectLoader->GetModel()->vIndices.size();

	glBindVertexArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}