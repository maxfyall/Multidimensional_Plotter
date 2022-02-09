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

#include "cube.h"

/* Include the GLM maths library for GLM functions along with matrix extensions	*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

GLuint program;
GLuint vArrayObj;

GLuint modelID, viewID, projectionID;

bool firstMouse = true;
bool moveScene = false;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 1024.f / 2.0;
float lastY = 768.f / 2.0;
float fov = 30.f;

glm::vec3 cameraPos = glm::vec3(0, 0, 3);
glm::vec3 cameraFront = glm::vec3(0, 0, -1);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);

Cube theCube;

GLfloat aspect_ratio;

// include namespaces to avoid std:: etc...
//using namespace std;
//using namespace glm;

/*
*  Initialising Function, called before rendering loop to initialise variables and creating objects
*/
void init(GLWrapper* glw)
{
	// define the aspect ratio used in the perspective call in display
	aspect_ratio = 1024.f / 768.f;
	
	glGenVertexArrays(1, &vArrayObj); // generate index (name) for one vertex array object
	glBindVertexArray(vArrayObj); // create the vertex array object and make it current

	// try to load in both vertex and fragment shaders
	try
	{
		// set program variable to loaded shaders
		program = glw->LoadShader("../../shaders/shaderVert.vert", "../../shaders/shaderFrag.frag");
	}
	catch (std::exception& e) // catch any exceptions
	{
		// print exception found
		std::cout << "Exception found: " << e.what() << std::endl;

		// ignore any input
		std::cin.ignore();
		exit(0); // exit program
	}

	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projectionID = glGetUniformLocation(program, "projection");

	theCube.makeCube();

}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);

	std::stack<glm::mat4> model;
	model.push(glm::mat4(1.0));

	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);

//	glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);

	glm::mat4 view = glm::lookAt(
		glm::vec3(0,0,4), 
		glm::vec3(0,0,0), 
		glm::vec3(0,1,0)
	);

	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);

	model.top() = glm::rotate(model.top(), -glm::radians(pitch), glm::vec3(1, 0.0f, 0.0f));
	model.top() = glm::rotate(model.top(), -glm::radians(pitch), glm::vec3(0.0f, 1, 0.0f));
	//model.top() = glm::rotate(model.top(), -glm::radians(pitch), glm::vec3(0.0f, 0.0f, cameraFront.z));


	model.push(model.top()); 
	{
		model.top() = glm::scale(model.top(), glm::vec3(1, 1, 1));
		model.top() = glm::translate(model.top(), glm::vec3(0, 0, 0));

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
		theCube.drawCube(0);
	}
}

/*
*  Resizing window callback
*/
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f * 4.f) / ((float)h / 480.f * 3.f);
}

/*
*  Monitor keyboard inputs
*/
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	// close application window with ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		std::cout << "ESC PRESSED - TERMINATING" << std::endl;
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == 'Q' && action == GLFW_PRESS)
	{
		std::cout << " MOUSE 1 " << std::endl;
		moveScene = !moveScene;
	} 
}

/*
*  Mouse Callback function aquired from Learn OpenGL
*/
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) 
{
	if (moveScene) 
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		double xoffset = xpos-lastX;
		double yoffset = lastY-ypos;
		lastX = xpos;
		lastY = ypos;

		cameraFront.x += xoffset;
		cameraFront.y += yoffset;

		pitch += (xoffset + yoffset) * 0.1f;


		//float sensitivity = 1.0f;
		//xoffset *= sensitivity;
		//yoffset *= sensitivity;
		//
		//yaw += xoffset;
		//pitch += yoffset;
		//
		//if (pitch > 89.0f)
		//	pitch = 89.0f;
		//if (pitch < -89.0f)
		//	pitch = -89.0f;
		//
		//glm::vec3 front;
		//
		//front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		//front.y = sin(glm::radians(pitch));
		//front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		//
		//cameraFront = front;
		//
		//cameraFront = glm::normalize(front);

	}

}

/*
*  Scroll Callback function acquired from Learn OpenGL
*/
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 30.f)
		fov = 30.f;
}


/*
*  Main Method, program control point
*/
int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(1024, 768, "Multidimensional Plotter");

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed, Terminating\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setMouseCallback(mouseCallback);
	glw->setScrollCallback(scrollCallback);
	glw->setReshapeCallback(reshape);

	glw->DisplayVersion();

	init(glw);

	glw->eventLoop();


	delete(glw);
	return 0;
}
