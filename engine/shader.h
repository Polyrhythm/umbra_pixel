#pragma once

#include <GL/glew.h>
#include <vector>

GLuint loadShader(char *filename, GLuint shaderId);
GLuint linkShaders(std::vector<GLuint> shaders, GLuint program);
