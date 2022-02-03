/*
	Max Fyall - 180011724
	Multidimensional Plotter
*/

/* Static Library Links */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glloadD.lib")
#else
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glload.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include GLFW wrapper class (Written by Dr. Iain Martin) for GLFW and OpenGL extensions */
#include "wrapper_glfw.h"

/* Include standard C++ libraries (Improves code readability) */
#include <iostream>
#include <stack>

/* Include the GLM maths library for GLM functions along with matrix extensions	*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

GLuint program;
GLuint vArrayObj;

// include namespaces to avoid std:: etc...
using namespace std;
using namespace glm;

void init(GLWrapper* glw)
{

}

void display()
{

}

static void reshape(GLFWwindow* window, int w, int h)
{

}

static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		cout << "ESC PRESSED - TERMINATING" << endl;
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(1024, 768, "Multidimensional Plotter");

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed, Terminating\n");
		return 0;
	}


	delete(glw);
	return 0;
}
