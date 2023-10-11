#pragma once
#ifndef MSHADER_H
#define MSHADER_H

#include <glad\glad.h>
#include <gltools.h>

class MShader
{
private:
	GLuint program;

public:
	MShader(const char* vs, const char* fs);

	void use();

	void setInt(const char* param_name, GLint val);
	void setFloat(const char* param_name, GLfloat val);
};
#endif // !MSHADER_H
