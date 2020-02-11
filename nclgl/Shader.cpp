#include "Shader.h"

Shader::Shader(std::string vertex, std::string fragement, std::string geometry) {
	program = glCreateProgram();
	objects[SHADER_VERTEX] = GenerateShader(vertex, GL_VERTEX_SHADER);
	objects[SHADER_FRAGEMENT] = GenerateShader(fragement, GL_FRAGMENT_SHADER);
	objects[SHADER_GEOMETRY] = 0;

	if (!geometry.empty()) {
		objects[SHADER_GEOMETRY] = GenerateShader(geometry, GL_GEOMETRY_SHADER);
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}

	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGEMENT]);
	SetDefaultAttributes();
}

Shader::~Shader() {

	for (int i = 0; i < 3; ++i) {
		glDetachShader(program, objects[i]);
		glDeleteShader(objects[i]);
	}
	glDeleteProgram(program);
}

bool Shader::LinkProgram() {

	if (loadFailed) {
		return false;
	}
	
	glLinkProgram(program);

	GLint code;

	glGetProgramiv(program, GL_LINK_STATUS, &code);
	if (code == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		for (auto x : infoLog)
			std::cout << x;
		return false;
	}
	return code == GL_TRUE ? true : false;
}

void Shader::SetDefaultAttributes() { 

	glBindAttribLocation(program, VERTEX_BUFFER, "position");
	glBindAttribLocation(program, COLOUR_BUFFER, "colour");
	glBindAttribLocation(program, NORMAL_BUFFER, "normal");
	glBindAttribLocation(program, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
	glBindAttribLocation(program, MODEL_BUFFER, "modelBuf");
}

bool Shader::LoadShaderFile(std::string from, std::string& into) {
	std::ifstream file;
	std::string temp;

	std::cout << "Loading shader from text " << from << std::endl;

	file.open(from.c_str());
	
	if (!file.is_open()) {
		std::cout << "File does not exist!" << std::endl;
		return false;
	}
	
	while (!file.eof()) {
		std::getline(file, temp);
		into += temp + "\n";
	}

	file.close();

	std::cout << into << std::endl;
	std::cout << "load shader text!" << std::endl;
	
	return true;
}

GLuint Shader::GenerateShader(std::string from, GLenum type) {
	std::cout << "Compiling Shader..." << std::endl;

	std::string load;
	if (!LoadShaderFile(from, load)) {
		std::cout << "Compiling failed!" << std::endl;
		loadFailed = true;
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		std::cout << "Compiling failed!" << std::endl;
		char error[512];
		glGetInfoLogARB(shader, sizeof(error), NULL, error);
		std::cout << error << std::endl;
		loadFailed = true;
		return 0;
	}

	std::cout << "Compiling success!" << std::endl;
	loadFailed = false;

	return shader;

}
