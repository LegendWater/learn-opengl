#pragma once
#ifndef MSHADER_H
#define MSHADER_H

#include <glad\glad.h>
#include <gltools.h>
#include <glm.hpp>
#include <gtc\type_ptr.hpp>
#include <typeinfo>

class MShader
{
private:
	GLuint program;

public:
	MShader(const char* vs, const char* fs);
	~MShader();

	void use();

	void setInt(const char* param_name, GLint val);
	void setFloat(const char* param_name, GLfloat val);
	template<glm::length_t N, typename T, glm::qualifier Q>
	void setVec(const char* param_name, const glm::vec<N, T, Q>& vals)
	{
		const auto& t = typeid(T);
		if (t == typeid(float)) {
			switch (N)
			{
			case 1: glUniform1fv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(vals)); break;
			case 2: glUniform2fv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(vals)); break;
			case 3: glUniform3fv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(vals)); break;
			case 4: glUniform4fv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(vals)); break;
			default: break;
			}
		}
		else if (t == typeid(int)) {
			glm::vec<N, int, Q> ivals = vals;
			switch (N)
			{
			case 1: glUniform1iv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(ivals)); break;
			case 2: glUniform2iv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(ivals)); break;
			case 3: glUniform3iv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(ivals)); break;
			case 4: glUniform4iv(glGetUniformLocation(program, param_name), 1, glm::value_ptr(ivals)); break;
			default: break;
			}
		}
		CHECK_GL_ERROR();
	}
	void setMat4(const char* param_name, glm::mat4 mat);
};
#endif // !MSHADER_H
