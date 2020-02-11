#pragma once

#include "OGLRenderer.h"

#define SHADER_VERTEX 0
#define SHADER_FRAGEMENT 1
#define SHADER_GEOMETRY 2

class Shader {
public:
	Shader(std::string vertex, std::string fragement, std::string geometry = "");
	~Shader();

	GLuint GetProgram() { return program; }
	bool LinkProgram();

protected:

	void SetDefaultAttributes();
	bool LoadShaderFile(std::string from, std::string& into);
	GLuint GenerateShader(std::string from, GLenum type);

	GLuint objects[3];
	GLuint program;

	bool loadFailed;
};