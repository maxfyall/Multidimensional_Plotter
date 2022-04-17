/**
	wrapper_glfw.h
	Modified from the OpenGL GLFW example to provide a wrapper GLFW class
	Iain Martin August 2014

	EXTENDED BY Max Fyall - 180011724
	Multidimensional Plotter - 2022
*/
#pragma once

#include <string>

/* Include header files for imgui */
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

/* Inlcude GL_Load and GLFW */
//#include <glload/gl_4_0.h>
//#include <glload/gl_load.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLFW Class
class GLWrapper {
private:

	int width;
	int height;
	const char *title;
	double fps;
	void(*renderer)();
	bool running;
	GLFWwindow* window;

public:
	GLWrapper(int width, int height, const char *title);
	~GLWrapper();

	void setFPS(double fps) {
		this->fps = fps;
	}

	// displays opengl and graphics driver versions
	void DisplayVersion();

	/* Callback registering functions */
	void setRenderer(void(*f)());
	void setReshapeCallback(void(*f)(GLFWwindow* window, int w, int h));
	void setKeyCallback(void(*f)(GLFWwindow* window, int key, int scancode, int action, int mods));
	void setErrorCallback(void(*f)(int error, const char* description));
	void setMouseCallback(void(*f)(GLFWwindow* window, double xpos, double ypos));
	void setMouseButtonCallback(void(*f)(GLFWwindow* window, int button, int action, int mods));
	void setScrollCallback(void(*f)(GLFWwindow* window, double xoffset, double yoffset));

	/* Shader load and build support functions */
	GLuint LoadShader(const char *vertex_path, const char *fragment_path);
	GLuint BuildShader(GLenum eShaderType, const std::string &shaderText);
	GLuint BuildShaderProgram(std::string vertShaderStr, std::string fragShaderStr);
	std::string readFile(const char *filePath);

	int eventLoop();
	GLFWwindow* getWindow();
};



