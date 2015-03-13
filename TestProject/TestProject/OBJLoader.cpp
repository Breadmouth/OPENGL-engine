#include <fstream>
#include "OBJLoader.h"

//OUTDATED


OBJLoader::OBJLoader(string objName)
{
	indexChecked = false;
	isQuads = false;

	hasUV = false;
	hasNormals = false;

	string fileName = "../objects/" + objName;
	LoadMaterials(fileName + ".mtl");
	LoadObjects(fileName + ".obj");

	m_numNormals = 0;
	m_numTexCoords = 0;
}

OBJLoader::~OBJLoader()
{

}

string OBJLoader::ParseString(string src, string code)
{
	char buffer[64];
	string scanStr = code + " &s";
	sscanf_s(src.c_str(), scanStr.c_str(), &buffer, _countof(buffer));
	return string(buffer);
}

glm::vec2 OBJLoader::ParseVec2(string src, string code) {
    float x, y;
    string scanStr = code + " %f %f";
    sscanf_s(src.c_str(), scanStr.c_str(), &x, &y);
    return glm::vec2(x, y);
}

glm::vec3 OBJLoader::ParseVec3(string src, string code) {
    float x, y, z;
    string scanStr = code + " %f %f %f";
    sscanf_s(src.c_str(), scanStr.c_str(), &x, &y, &z);
    return glm::vec3(x, y, z);
}

void OBJLoader::AddIndices(string str) {
    int v1 = -1, u1 = -1, n1 = -1,
        v2 = -1, u2 = -1, n2 = -1,
        v3 = -1, u3 = -1, n3 = -1,
        v4 = -1, u4 = -1, n4 = -1;

    CheckIndices(str);

    if (!hasUV && !hasNormals) {
        sscanf_s(str.c_str(),"f %d %d %d %d",
            &v1,
            &v2,
            &v3,
            &v4);
    }
    else if (hasUV && !hasNormals) {
        sscanf_s(str.c_str(),"f %d/%d %d/%d %d/%d %d/%d",
            &v1, &u1,
            &v2, &u2,
            &v3, &u3,
            &v4, &u4);
    }
    else if (!hasUV && hasNormals) {
        sscanf_s(str.c_str(),"f %d//%d %d//%d %d//%d %d//%d",
            &v1, &n1,
            &v2, &n2,
            &v3, &n3,
            &v4, &n4);
    }
    else if (hasUV && hasNormals) {
        sscanf_s(str.c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
            &v1, &u1, &n1,
            &v2, &u2, &n2,
            &v3, &u3, &n3,
            &v4, &u4, &n4);
    }

    v1--; u1--; n1--;
    v2--; u2--; n2--;
    v3--; u3--; n3--;
    v4--; u4--; n4--;

    if (v3 >= 0) {
        float r = currentMaterial->colour.r;
        float g = currentMaterial->colour.g;
        float b = currentMaterial->colour.b;

        object->vIndices.push_back(v1);
        object->vIndices.push_back(v2);
        object->vIndices.push_back(v3);
        if (v4 >= 0) {
            object->vIndices.push_back(v4);
            isQuads = true;
        }
    }

    if (u3 >= 0) {
		hasTexCoord = true;
        object->uIndices.push_back(u1);
        object->uIndices.push_back(u2);
        object->uIndices.push_back(u3);
        if (u4 >= 0) {
            object->uIndices.push_back(u4);
        }
    }

    if (n3 >= 0) {
		hasNormal = true;
        object->nIndices.push_back(n1);
        object->nIndices.push_back(n2);
        object->nIndices.push_back(n3);
        if (n4 >= 0) {
            object->nIndices.push_back(n4);
        }
    }
}

void OBJLoader::CheckIndices(string str) {
    if (!indexChecked) {
        int bestSample = 0;
        int sampleV1 = -1, sampleU1 = -1, sampleN1 = -1;
        int tmp = -1, sampleV4 = -1;

        int form1 = sscanf_s(str.c_str(),"f %d", &sampleV1);
        int form2 = sscanf_s(str.c_str(),"f %d/%d", &sampleV1, &sampleU1);
        int form3 = sscanf_s(str.c_str(),"f %d//%d", &sampleV1, &sampleN1);
        int form4 = sscanf_s(str.c_str(),"f %d/%d/%d", &sampleV1, &sampleU1, &sampleN1);

        if (form1 > bestSample) {
            bestSample = form1;
            hasUV = false;
            hasNormals = false;
        }
        if (form2 > bestSample) {
            bestSample = form2;
            hasUV = true;
            hasNormals = false;
        }
        if (form3 > bestSample) {
            bestSample = form3;
            hasUV = false;
            hasNormals = true;
        }
        if (form4 > bestSample) {
            bestSample = form4;
            hasUV = true;
            hasNormals = true;
        }

        indexChecked = true;
    }
}

void OBJLoader::LoadObjects(string objPath) {
    ifstream file(objPath);
    string buffer;
    object = new ObjectData();
    currentMaterial = materials[0];

    if (file.is_open()) {
        while (!file.eof()) {
            getline(file, buffer);

            if (buffer.find("v ") == 0) {
                object->vertices.push_back(ParseVec3(buffer, "v"));
				object->colors.push_back(currentMaterial->colour);
            }
            else if (buffer.find("vn") == 0) {
                object->normals.push_back(ParseVec3(buffer, "vn"));
				m_normals[++m_numNormals] = ParseVec3(buffer, "vn");
            }
            else if (buffer.find("vt") == 0) {
                object->texCoords.push_back(ParseVec2(buffer, "vt"));
				m_texCoords[++m_numTexCoords] = ParseVec2(buffer, "vt");
            }
            else if (buffer.find("usemtl") == 0) {
                string name = ParseString(buffer, "usemtl");
                for (int i = 0; i < materials.size(); i++) {
                    if (name == materials[i]->name) {
                        currentMaterial = materials[i];
                    }
                }
            }
            else if (buffer.find("f ") != string::npos) {
                AddIndices(buffer);
            }
        }
    }
    else {
        printf("Unable to find: %s\n", objPath.c_str());
    }
}

void OBJLoader::LoadMaterials(string matPath) {
    ifstream file(matPath);
    string buffer;
    currentMaterial = NULL;

    if (file.is_open()) {
        while (!file.eof()) {
            getline(file, buffer);

            if (buffer.find("newmtl") == 0) {
                if (currentMaterial != NULL) {
                    materials.push_back(currentMaterial);
                }
                currentMaterial = new Material();
                currentMaterial->name = ParseString(buffer, "newmtl");
            }
            else if (buffer.find("Kd") == 0) {
                currentMaterial->colour = ParseVec3(buffer, "Kd");
            }
        }
    }
    else {
        printf("Unable to find: %s\n", matPath.c_str());
    }

    if (currentMaterial == NULL) {
        currentMaterial = new Material();
        currentMaterial->colour = glm::vec3(1, 1, 1);
    }

    materials.push_back(currentMaterial);
}

ObjectData* OBJLoader::GetModel() {
    return object;
}

GLenum OBJLoader::GetRenderMode() {
    GLenum mode = ((isQuads) ? GL_QUADS : GL_TRIANGLES);
    return mode;
}

bool OBJLoader::IsQuads()
{
	if (isQuads)
		return true;
	else
		return false;
}

glm::vec2 OBJLoader::GetTexCoord(GLuint key)
{
	return m_texCoords[key];
}

glm::vec3 OBJLoader::GetNormal(GLuint key)
{
	return m_normals[key];
}