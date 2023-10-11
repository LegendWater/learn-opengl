#include "MShader.h"

MShader::MShader(const char* vs, const char* fs)
{
	program = genGLShaderProgram(vs, fs);
}

void MShader::use()
{
	glUseProgram(program);
}

void MShader::setInt(const char* param_name, GLint val)
{
	glUniform1i(glGetUniformLocation(program, param_name), val);
}

void MShader::setFloat(const char* param_name, GLfloat val)
{
	glUniform1f(glGetUniformLocation(program, param_name), val);
}
