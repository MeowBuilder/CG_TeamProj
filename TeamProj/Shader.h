#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

class Shader
{
public:
	char* File_To_Buf(const char* file);
	bool Make_Shader_Program();

	GLuint shaderProgramID;
};

