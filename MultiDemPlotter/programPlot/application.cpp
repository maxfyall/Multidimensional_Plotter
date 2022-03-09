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
#include <fstream>
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>


#include "cube.h"

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

GLfloat largest;
GLuint colourMode;

Cube theCube;

GLfloat aspect_ratio;

std::vector<float> vertexPos;

GLfloat xAxesColour[]
{
	1.0f, 0.0f, 0.0f, 1.0f, 
	1.0f, 0.0f, 0.0f, 1.0f
};

GLfloat yAxesColour[]
{
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f
};

GLfloat zAxesColour[]
{
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
};

//GLfloat plotPositions[23];


std::vector<float> read3DData(const char *filePath);

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

	GLfloat xAxesVertex[] =
	{
		largest + 1, 0.0f, 0.0f,
		-(largest + 1), 0.0f, 0.0f
	};

	GLfloat yAxesVertex[]
	{
		0.0f, (largest+1.f), 0.f,
		0.0f, -(largest + 1.f), 0.0f
	};

	GLfloat zAxesVertex[]
	{
		0.0f, 0.0f, (largest + 1.f),
		0.0f, 0.0f, -(largest + 1.f)
	};


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

	glGenBuffers(1, &xAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesVertex), xAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &xColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesColour), xAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &yAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesVertex), yAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &yColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesColour), yAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &zAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesVertex), zAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &zColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesColour), zAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &plotBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float) , &vertexPos.front() , GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//delete[] plotPositions;

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
		
		glLineWidth(5.0f);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model.top()[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_LINES, 0, 2);

		glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_LINES, 0, 2);

		glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);


		glDrawArrays(GL_LINES, 0, 2);
		
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

		if (size/3 >= 1)
		{
			glDrawArrays(GL_POINTS, 0, size / 3);
		}

		colourMode = 0;
		glUniform1ui(colourModeID, colourMode);

	}
	model.pop();

	ImGui::Begin("MULTIDIMENSIONAL PLOTTER");
	ImGui::Text("This is an ImGui window");
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

	if (key == 'M' && action == GLFW_PRESS)
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

		double xoffset = xpos-lastX;
		double yoffset = lastY-ypos;
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
std::vector<float> read3DData(const char *filePath) 
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

	// start loop to add data to data structure
	//for (int i = 0; i < vertexPositions.size(); i++) 
	//{
	//	// https://www.tutorialspoint.com/how-to-remove-certain-characters-from-a-string-in-cplusplus

	//	//vertexPositions[i].erase(remove(vertexPositions[i].begin(), vertexPositions[i].end(), ' '), vertexPositions[i].end());

	//	std::cout << "New String : "<< vertexPositions[i] << std::endl;

	//	std::string temp;
	//	std::string value;

	//	int j = 0;

	//	while (j <= vertexPositions[i].length()-1)
	//	{
	//		std::cout << vertexPositions[i].length() << std::endl;

	//		if (vertexPositions[i].empty()) 
	//		{
	//			std::cout << "EMPTY LINE: BREAKING FROM WHILE LOOP" << std::endl;
	//			break;
	//		}

	//		temp = vertexPositions[i].at(j);

	//		std::cout << "NEXT STRING ELEMENT: " << temp << std::endl;

	//		std::cout << "Counter: "<< j << std::endl;

	//		// are we dealing with a negative number?
	//		if (temp == "-")
	//		{
	//			while (vertexPositions[i].at(j) != ' ')
	//			{
	//				value = value + vertexPositions[i].at(j);
	//				j++;
	//				if (j == vertexPositions[i].length())
	//				{
	//					break;
	//				}
	//			}

	//			plotPos.push_back(std::stof(value));
	//			std::cout << "Added Negative Number " << value << std::endl;
	//			j++;
	//		}
	//		else if (j != vertexPositions[i].length()-1)
	//		{
	//			// are we dealing with a decimal number
	//			if (vertexPositions[i].at(j + 1) == '.')
	//			{
	//				//temp = temp + vertexPositions[i].at(j+1);
	//				while (vertexPositions[i].at(j) != ' ')
	//				{
	//					value = value + vertexPositions[i].at(j);
	//					j++;
	//					if (j == vertexPositions[i].length())
	//					{
	//						break;
	//					}
	//				}

	//				plotPos.push_back(std::stof(value));
	//				std::cout << "Added Decimal Number " << value << std::endl;
	//				j++;
	//			}
	//			
	//			else if (temp == " ")
	//			{
	//				j++;
	//			}
	//			else
	//			{
	//				value = temp;
	//				std::cout << "Added Number " << value << std::endl;
	//				plotPos.push_back(std::stof(value));
	//				j++;
	//			}
	//		}
	//	
	//		else if (temp == " ")
	//		{
	//			j++;
	//		}
	//		else
	//		{
	//			value = temp;
	//			std::cout << "Added Number " << value << std::endl;
	//			plotPos.push_back(std::stof(value));
	//			j++;
	//		}
	//	

	//		if (value != "")
	//		{
	//			if (std::stof(value) > largest)
	//			{
	//				largest = std::stof(value);
	//			}
	//		}

	//		value.clear();
	//		temp.clear();
	//		
	//	}
	//}

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
