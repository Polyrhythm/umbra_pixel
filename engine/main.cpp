#include "main.h"
#include "shader.h"
#include "file.h"

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

unsigned int windowWidth = 1024;
unsigned int windowHeight = 768;

FileWatcher* watcher;

// glUniforms
GLint timeLoc;
GLint resLoc;
GLint deltaLoc;

// Performance vars
Uint64 lastFrame;
Uint64 nowFrame;


bool Init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Failed to init SDL\n";
		return false;
	}

	mainWindow = SDL_CreateWindow(
		programName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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

	// Watch for changes in frag shader.
	watcher = new FileWatcher("fragment_shader.frag");

	// Set up VAO, VBO, buffers. Render stuff.
	drawScene();
	linkShaders(std::vector<GLuint> {vertShader, fragShader}, glProgram);
	Render();

	// Cleanup only executed if render loop is broken.
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

void assignUniforms() {
	if (timeLoc != -1) {
		glUniform1f(timeLoc, SDL_GetTicks() * 0.001);
	}

	if (resLoc != -1) {
		glUniform2f(resLoc, windowWidth, windowHeight);
	}

	if (deltaLoc != -1) {
		glUniform1f(
			deltaLoc,
			(double)((nowFrame - lastFrame) * 1000 / SDL_GetPerformanceFrequency())
		);
	}
}

void Render() {
	bool loop = true;
	lastFrame = 0;
	nowFrame;

	glClearColor(0.0, 0.0, 0.0, 1.0);

	timeLoc = glGetUniformLocation(glProgram, "time");
	resLoc = glGetUniformLocation(glProgram, "resolution");
	deltaLoc = glGetUniformLocation(glProgram, "deltaTime");

	while (loop) {
		nowFrame = SDL_GetPerformanceCounter();

		// Check for changes on frag shader.
		if (watcher->fileChanged()) {
			// Reload frag shader.
			if (loadShader("fragment_shader.frag", fragShader)) {
				linkShaders(std::vector<GLuint> {fragShader}, glProgram);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(glProgram);

		assignUniforms();
		
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

			else if (event.type == SDL_KEYDOWN) {
				const Uint8 *keystate = SDL_GetKeyboardState(NULL);
				
				if (keystate[SDL_SCANCODE_ESCAPE]) {
					loop = false;
					break;
				}
			}

			else if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					windowWidth = event.window.data1;
					windowHeight = event.window.data2;
					glViewport(0, 0, windowWidth, windowHeight);

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