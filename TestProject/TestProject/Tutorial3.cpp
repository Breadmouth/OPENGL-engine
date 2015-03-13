#include "Tutorial3.h"
#include <Gizmos.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJLoader.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Tutorial3::Tutorial3()
{
	light = vec3(1, 1, 0);
}

void Tutorial3::Create()
{
	camera.SetInputWindow(glfwGetCurrentContext());

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load("../data/textures/crate.png",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
	
	//AntTweakBar
	m_bar = TwNewBar("my bar");
	TwAddVarRW(m_bar, "clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "light dir", TW_TYPE_DIR3F, &light, "");

	LoadShader();
	//CreateShader();

	LoadObject("Stone Bridge/bridge");
}

void Tutorial3::Destroy()
{

}

void Tutorial3::Update(float dt)
{


	camera.Update(dt);
}

void Tutorial3::Draw()
{
	glUseProgram(m_programID);

	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(camera.GetProjectionView()));

	unsigned int lightUniform = glGetUniformLocation(m_programID, "light");
	glUniform3fv(lightUniform, 1, glm::value_ptr(light));
	
	unsigned int cameraPosUniform = glGetUniformLocation(m_programID, "cameraPos");
	glUniform3fv(cameraPosUniform, 1, glm::value_ptr(camera.GetPosition()));
	
	unsigned int lightColorUniform = glGetUniformLocation(m_programID, "lightColor");
	glUniform3fv(lightColorUniform, 1, glm::value_ptr(vec3(1, 1, 1)));
	
	unsigned int specPowUniform = glGetUniformLocation(m_programID, "specPow");
	float specPow = 128.0f;
	glUniform1fv(specPowUniform, 1, &specPow);

	//set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	//tell the shader where it is
	unsigned int loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);

	glBindVertexArray(m_VAO);

	if (objectLoader->IsQuads())
		glDrawElements(GL_QUADS, indexCount, GL_UNSIGNED_INT, 0);
	else
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);


	Gizmos::draw(camera.GetProjectionView());

}

void Tutorial3::CreateSquare()
{
	float vertexData[] = {
		-5, 0, 5, 1, 0, 1,
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
		-5, 0, -5, 1, 0, 0,
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indexData, GL_STATIC_DRAW);
	

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	indexCount = 6;
}

void Tutorial3::CreateShader()
{
	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 TexCoord; \
						   out vec2 vTexCoord; \
						   uniform mat4 ProjectionView; \
						   void main() \
						   { \
								vTexCoord = TexCoord; \
								gl_Position = ProjectionView * Position; \
						   }";

	const char* fsSource = "#version 410\n \
						   in vec2 vTexCoord; \
						   out vec4 FragColor; \
						   uniform sampler2D diffuse; \
						   void main() \
						   { \
								FragColor = texture(diffuse, vTexCoord); \
						   }" ;

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

void Tutorial3::LoadObject(char *path)
{
	std::vector<Vertex> vertex;

	objectLoader = new OBJLoader(path);

	for (int i = 0; i < objectLoader->GetModel()->vertices.size(); ++i)
	{
		Vertex vert;
		vert.position = vec4(objectLoader->GetModel()->vertices[i], 1);

		if (i < objectLoader->GetModel()->normals.size())
			vert.normal = vec4(objectLoader->GetModel()->normals[i], 1);
		else
			vert.normal = vec4(0, 0, 0, 1);

		if (i < objectLoader->GetModel()->texCoords.size())
			vert.uv = vec2(objectLoader->GetModel()->texCoords[i]);
		else
			vert.uv = vec2(0, 0);

		vertex.push_back(vert);
	}

	//for (int i = 0; i < objectLoader->GetModel()->vertices.size(); ++i)
	//{
	//	Vertex vert;
	//	vert.position = vec4(objectLoader->GetModel()->vertices[i], 1);
	//	vert.normal = vec4(0, 0, 0, 1);
	//	vert.uv = vec2(0, 0);
	//
	//	vertex.push_back(vert);
	//}
	//
	//for (int i = 0; i < objectLoader->GetModel()->nIndices.size(); ++i)
	//{
	//	Vertex vert;
	//
	//	if (i < vertex.size())
	//		vert = vertex[i];
	//
	//	if (i < objectLoader->GetModel()->normals.size())
	//	{
	//		if (objectLoader->hasNormal)
	//			vert.normal = vec4(objectLoader->GetNormal(objectLoader->GetModel()->nIndices[i]), 1);
	//		else
	//			vert.normal = vec4(objectLoader->GetModel()->normals[i], 1);
	//	}
	//
	//	if (i < vertex.size())
	//		vertex[i] = vert;
	//	else
	//		vertex.push_back(vert);
	//}
	//
	//for (int i = 0; i < objectLoader->GetModel()->uIndices.size(); ++i)
	//{
	//	Vertex vert;
	//
	//	if (i < vertex.size())
	//		vert = vertex[i];
	//
	//	if (i < objectLoader->GetModel()->texCoords.size())
	//	{
	//		if (objectLoader->hasTexCoord)
	//			vert.uv = vec2(objectLoader->GetTexCoord(objectLoader->GetModel()->uIndices[i]));
	//		else
	//			vert.uv = vec2(objectLoader->GetModel()->texCoords[i]);
	//	}
	//
	//	if (i < vertex.size())
	//		vertex[i] = vert;
	//	else
	//		vertex.push_back(vert);
	//}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	
	glGenVertexArrays(1, &m_VAO);
	
	glBindVertexArray(m_VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) * 2));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objectLoader->GetModel()->vIndices.size() * sizeof(GLuint), &objectLoader->GetModel()->vIndices[0], GL_STATIC_DRAW);
	indexCount = objectLoader->GetModel()->vIndices.size();

	glBindVertexArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Tutorial3::LoadShaderFromFile(std::string path, GLenum shaderType)
{
	GLuint shaderID = 0;
	string shaderString;
	std::ifstream sourceFile( path.c_str() );

	//sourcefile loaded
	if (sourceFile)
	{
		//get shader source
		shaderString.assign((istreambuf_iterator<char>(sourceFile)), istreambuf_iterator<char>() );
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
			printf ("unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
			glDeleteShader(shaderID);
			shaderID = 0;
		}
	}
	else
	{
		printf ("unable to open file %s\n", path.c_str());
	}

	return shaderID;
}

bool Tutorial3::LoadShader()
{
	//Generate program
    m_programID = glCreateProgram();

    //Load vertex shader
    GLuint vertexShader = LoadShaderFromFile( "../data/shaders/shader3.glvs", GL_VERTEX_SHADER );

    //Check for errors
    if( vertexShader == 0 )
    {
        glDeleteProgram( m_programID );
        m_programID = 0;
        return false;
    }

    //Attach vertex shader to program
    glAttachShader( m_programID, vertexShader );


    //Create fragment shader
    GLuint fragmentShader = LoadShaderFromFile( "../data/shaders/shader3.glfs", GL_FRAGMENT_SHADER );

    //Check for errors
    if( fragmentShader == 0 )
    {
        glDeleteShader( vertexShader );
        glDeleteProgram( m_programID );
        m_programID = 0;
        return false;
    }

    //Attach fragment shader to program
    glAttachShader( m_programID, fragmentShader );

    //Link program
    glLinkProgram( m_programID );

    //Clean up excess shader references
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    return true;
}