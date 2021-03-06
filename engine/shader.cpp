#include "shader.h"
#include <string>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include "file.h"

using std::string;

bool loadShader(char *filename, GLuint shader) {
	char* shaderSource = readTextFile(filename);
	GLint result = GL_FALSE;

	string preamble = "#version 330 core\nout vec4 outColour;\n";
	GLchar const* files[] = { preamble.c_str(), shaderSource };
	glShaderSource(shader, 2, files, 0);

	std::cout << "Compiling shader..." << std::endl;
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int logLength;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> shaderLog((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(shader, logLength, NULL, &shaderLog[0]);
		std::cout << &shaderLog[0] << std::endl;

		return false;
	}

	return true;
}

GLuint linkShaders(std::vector<GLuint> shaders, GLuint program) {
	GLint result = GL_FALSE;

	using Iter = std::vector<GLuint>::const_iterator;
	for (Iter iter = shaders.begin(); iter != shaders.end(); ++iter) {
		glAttachShader(program, *iter);
	}

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		int logLength;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> programLog((logLength > 1) ? logLength : 1);
		glGetProgramInfoLog(program, logLength, NULL, &programLog[0]);
		std::cout << &programLog[0] << std::endl;
	}

	for (Iter iter = shaders.begin(); iter != shaders.end(); ++iter) {
		glDeleteShader(*iter);
	}

	return program;
}