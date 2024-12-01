#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma once

class Object
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;

	bool Load_Object(const char* path);

public:
	GLuint VAO, VBO, EBO;
	
	bool Set_Obj(GLuint shaderProgramID, const char* path);
	void Draw(GLuint shaderProgramID);
	void Update();

};

