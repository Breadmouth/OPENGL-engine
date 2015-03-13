#pragma once
#include <vector>
#include <map>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <include\GLFW\glfw3.h>

using namespace std;

//OUTDATED


struct ObjectData 
{
    vector <glm::vec3> vertices, normals, colors;
    vector <glm::vec2> texCoords;
    vector <GLuint> vIndices, uIndices, nIndices;
};

class OBJLoader
{
public:
	OBJLoader(std::string objName);
	~OBJLoader();

	ObjectData* GetModel();
	GLenum GetRenderMode();

	bool IsQuads();
	bool hasTexCoord;
	bool hasNormal;

	glm::vec2 GetTexCoord(GLuint key);
	glm::vec3 GetNormal(GLuint key);


private:
	struct Material {
		std::string name;
		glm::vec3 colour;
	};

	std::vector <Material*> materials;
	Material *currentMaterial;
	ObjectData *object;

	GLuint m_numTexCoords;
	GLuint m_numNormals;

	std::map<GLuint, glm::vec2> m_texCoords;
	std::map<GLuint, glm::vec3> m_normals;

	
	bool hasUV, hasNormals, isQuads, indexChecked;

	string ParseString(string src, string code);
	glm::vec2 ParseVec2(string src, string code);
	glm::vec3 ParseVec3(string src, string code);
	void AddIndices(string str);
	void CheckIndices(string str);
	void LoadObjects(string objPath);
	void LoadMaterials(string matPath);

};