#include "main.h"
#include "shader.h"

#include <string>
#include <iostream>

#define GL3_PROTOTYPES 1
#include <GL/glew.h>

#include <SDL2/SDL.h>

const std::string programName = "UmbraPixel";

SDL_Window *mainWindow;
SDL_GLContext mainContext;
GLuint fragShader, vertShader;
GLuint glProgram;
GLuint vao, vbo[1];
int coordinateLocation = 0;
GLsizei stride = 0;
void* offset = 0;

const unsigned int WINDOW_WIDTH = 1024;
const unsigned int WINDOW_HEIGHT = 768;

bool Init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Failed to init SDL\n";
		return false;
	}

	mainWindow = SDL_CreateWindow(
		programName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL
	);

	if (!mainWindow) {
		std::cout << "Unable to create window\n";
		CheckSDLError(__LINE__);
		return false;
	}

	mainContext = SDL_GL_CreateContext(mainWindow);

	SetOpenGLAttributes();
	PrintSDL_GL_Attributes();
	SDL_GL_SetSwapInterval(1);

	#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
	#endif // !__APPLE__

	return true;
}

bool SetOpenGLAttributes() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	return true;
}

int main(int argc, char *argv[]) {
	if (!Init()) {
		return -1;
	}

	glProgram = glCreateProgram();

	// Create and link shaders.
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	loadShader("vertex_shader.vert", vertShader);
	loadShader("fragment_shader.frag", fragShader);

	drawScene();

	linkShaders(std::vector<GLuint> {vertShader, fragShader}, glProgram);

	Render();
	Cleanup();

	return 0;
}

void drawScene() {
	static const GLfloat quad[6][3] = {
		{ -1.0f, -1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f },
		{ -1.0f,  1.0f, 0.0f },
		{ -1.0f,  1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f },
		{  1.0f,  1.0f, 0.0f }
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), quad, GL_STATIC_DRAW);
	glVertexAttribPointer(coordinateLocation, 3, GL_FLOAT, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(0);
}

void Render() {
	bool loop = true;

	glClearColor(0.0, 0.0, 0.0, 1.0);

	GLint timeLoc = glGetUniformLocation(glProgram, "time");
	GLint resLoc = glGetUniformLocation(glProgram, "resolution");

	while (loop) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(glProgram);

		if (timeLoc != -1) {
			glUniform1f(timeLoc, SDL_GetTicks() * 0.001);
		}

		if (resLoc != -1) {
			glUniform2f(resLoc, WINDOW_WIDTH, WINDOW_HEIGHT);
		}

		std::cout << timeLoc << " : " << SDL_GetTicks() * 0.001 << std::endl;
		std::cout << resLoc << " : " << WINDOW_WIDTH << " / " << WINDOW_HEIGHT << std::endl;

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(coordinateLocation, 3, GL_FLOAT, GL_FALSE, stride, offset);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);

		SDL_GL_SwapWindow(mainWindow);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				loop = false;
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						loop = false;
						break;

					default:
						break;
				}
			}
		}
	}
}

void Cleanup() {
	glUseProgram(0);
	glDisableVertexAttribArray(0);
	glDetachShader(glProgram, vertShader);
	glDetachShader(glProgram, fragShader);
	glDeleteProgram(glProgram);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	glDeleteBuffers(1, vbo);
	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(mainContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}

void CheckSDLError(int line = -1) {
	std::string error = SDL_GetError();

	if (error != "") {
		std::cout << "SDL Error: " << error << std::endl;

		if (line != -1) {
			std::cout << "Line: " << line << std::endl;
		}

		SDL_ClearError();
	}
}

void PrintSDL_GL_Attributes() {
	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "SDL_GL_MAJOR_VERSION: " << value << std::endl;

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "SDL_GL_MINOR_VERSION: " << value << std::endl;
}