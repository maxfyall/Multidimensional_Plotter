/*
	Max Fyall - 180011724
	Multidimensional Plotter
*/

#define UNICODE
#pragma comment(lib, "user32.lib")

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
#include <fstream>
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <commdlg.h>


#include "axes.h"

/* Include the GLM maths library for GLM functions along with matrix extensions	*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

GLuint program;
GLuint vArrayObj;
GLuint xAxesBufferObject, xColourBuffer;
GLuint yAxesBufferObject, yColourBuffer;
GLuint zAxesBufferObject, zColourBuffer;
GLuint plotBufferObject;

GLuint modelID, viewID, projectionID, colourModeID;

bool firstMouse = true;
bool moveScene = false;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 1024.f / 2.0;
float lastY = 768.f / 2.0;
float fov = 45.f;
float scaler = 0.5f;

int size;
bool clear;

GLfloat largest;
GLuint colourMode;

ThreeDAxes newAxes;

GLfloat aspect_ratio;

std::vector<float> vertexPos;

std::vector<float> read3DData(const char* filePath);

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

	// read in data to a float vector

	vertexPos = read3DData("../../testData/test3DData.txt");

	std::cout << "Largest Value: " << largest << std::endl;

	// create axes with the largest number from data set.
	newAxes.makeAxes(largest);

	size = vertexPos.size();

	for (int i = 0; i < vertexPos.size(); i++)
	{
		std::cout << vertexPos[i] << " ";
	}

	std::cout << std::endl;

	std::cout << vertexPos.size() << std::endl;

	modelID = glGetUniformLocation(program, "model");
	colourModeID = glGetUniformLocation(program, "colourMode");
	viewID = glGetUniformLocation(program, "view");
	projectionID = glGetUniformLocation(program, "projection");

	glGenBuffers(1, &plotBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &vertexPos.front(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 0, 4),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	//view = glm::rotate(view, -glm::radians(yaw), glm::vec3(1, 0, 0));
	//view = glm::rotate(view, glm::radians(20.0f), glm::vec3(1, 0, 0));

	//model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
	//model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniform1ui(colourModeID, colourMode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);

	model.top() = glm::scale(model.top(), glm::vec3(scaler, scaler, scaler));

	model.push(model.top());
	{

		model.top() = glm::scale(model.top(), glm::vec3(1, 1, 1));

		//model.top() = glm::translate(model.top(), glm::vec3(1, 0, 0));
		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 0.0f, 1.0f));

		newAxes.drawAxes();

	}
	model.pop();

	model.push(model.top());
	{

		//model.top() = glm::scale(model.top(), glm::vec3(0.1, 0.1, 0.1));
		//model.top() = glm::translate(model.top(), glm::vec3(-1, -1, -1));

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		colourMode = 1;
		glUniform1ui(colourModeID, colourMode);
		glPointSize(10.0f);

		if (size / 3 >= 1)
		{
			glDrawArrays(GL_POINTS, 0, size / 3);
		}
		else if (size / 2) 
		{
			glDrawArrays(GL_POINTS, 0, size / 2);
		}

		colourMode = 0;
		glUniform1ui(colourModeID, colourMode);

	}
	model.pop();

	ImGui::Begin("MULTIDIMENSIONAL PLOTTER");
	
	ImGui::Text("This is an ImGui window");

	if (ImGui::Button("Open")) 
	{
		//https://www.youtube.com/watch?v=-iMGhSlvIR0
		//https://docs.microsoft.com/en-us/answers/questions/483237/a-value-of-type-34const-char-34-cannot-be-assigned.html

		OPENFILENAME ofn;

		wchar_t file_name[MAX_PATH];
		const wchar_t spec[] = L"All files\0 *.*\0";

		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetFocus();
		ofn.lpstrFile = file_name;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = spec;
		ofn.nFilterIndex = 1;
		
		GetOpenFileName(&ofn);
	}

	if (ImGui::Button("Clear Graph"))
	{
		std::cout << "CLEAR THE GRAPH" << std::endl;
		vertexPos.clear();
		vertexPos.push_back(0);

		size = vertexPos.size();

		glGenBuffers(1, &plotBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &(vertexPos[0]), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		if (moveScene)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

	}

}

/*
*  Mouse Callback function aquired from Learn OpenGL and StackOverflow
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

		double xoffset = xpos - lastX;
		double yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		pitch += xoffset * 0.1f;
		yaw += yoffset * 0.1f;

	}

}

/*
*  Scroll Callback function acquired from Learn OpenGL
*/
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// need to edit to a scale value variable to prevent aliasing effects
	fov -= (float)yoffset;
	if (fov < 1.f)
		fov = 1.f;
	if (fov > 45.f)
		fov = 45.f;
}

/*
*  Read vertex positions from file returns vector of floats to be copied into dynamic array,
*  Also finds the largest value for creating 3-D axes to fit points provided
*/
std::vector<float> read3DData(const char* filePath)
{
	std::vector<std::string> vertexPositions;

	std::ifstream filestream(filePath);

	if (!filestream.is_open())
	{
		std::cout << "Could not read data file: " << filePath << ". File does not exist. " << std::endl;
		std::vector<float> empty;
		return empty;
	}

	std::string line;
	while (!filestream.eof())
	{
		getline(filestream, line);
		vertexPositions.push_back(line);
	}

	filestream.close();

	std::vector<float> plotPos;

	// https://www.geeksforgeeks.org/how-to-split-a-string-in-cc-python-and-java/

	for (int i = 0; i < vertexPositions.size(); i++)
	{
		std::string temp = vertexPositions[i];
		std::string num;

		std::stringstream ss(temp);

		while (ss >> num)
		{
			std::cout << num << std::endl;

			plotPos.push_back(std::stof(num));

			if (std::stof(num) > largest)
			{
				largest = std::stof(num);
			}

		}
	}

	if (plotPos.empty())
	{
		std::vector<float> check;
		check.push_back(0);
		return check;
	}
	else
	{
		return plotPos;
	}
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
