#include "GlShader.h"

using namespace Demobox;

#include <GL/glew.h>

GlShader::GlShader()
	: shaderProgramId_(0)
{
	shaderProgramId_ = glCreateProgram();
}

GlShader::~GlShader()
{
}

void GlShader::setVertexShader(const std::string& src)
{
	const char* str = src.c_str();
	uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &str, nullptr);
	glCompileShader(vs);
	glAttachShader(shaderProgramId_, vs);
}

void GlShader::setFragmentShader(const std::string& src)
{
	const char* str = src.c_str();
	uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &str, nullptr);
	glCompileShader(fs);
	glAttachShader(shaderProgramId_, fs);
}

void GlShader::build()
{
	glLinkProgram(shaderProgramId_);
}

void GlShader::bind()
{
	glUseProgram(shaderProgramId_);
}

void GlShader::release()
{
	glUseProgram(0);
}

void GlShader::setUniform(const std::string& name, float v1)
{
	glUniform1f(glGetUniformLocation(shaderProgramId_, name.c_str()), v1);
}

void GlShader::setUniform(const std::string& name, int v1)
{
	glUniform1i(glGetUniformLocation(shaderProgramId_, name.c_str()), v1);
}

void GlShader::setUniform(const std::string& name, float v1, float v2)
{
	glUniform2f(glGetUniformLocation(shaderProgramId_, name.c_str()), v1, v2);
}

void GlShader::setUniform(const std::string& name, float v1, float v2, float v3)
{
	glUniform3f(glGetUniformLocation(shaderProgramId_, name.c_str()), v1, v2, v3);
}

void GlShader::setUniform(const std::string& name, float v1, float v2, float v3, float v4)
{
	glUniform4f(glGetUniformLocation(shaderProgramId_, name.c_str()), v1, v2, v3, v4);
}
