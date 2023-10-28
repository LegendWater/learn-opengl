#include "MShader.h"

MShader::MShader(const char* vs, const char* fs)
{
	program = genGLShaderProgram(vs, fs);
}

MShader::~MShader()
{
	glDeleteProgram(program);
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

void MShader::setMat4(const char* param_name, glm::mat4 mat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, param_name), 1, GL_FALSE, glm::value_ptr(mat));
}
