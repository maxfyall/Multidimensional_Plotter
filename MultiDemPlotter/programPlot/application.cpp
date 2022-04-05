/*
	Max Fyall - 180011724
	Multidimensional Plotter
*/

/*

	Freetype Code Reference - https://learnopengl.com/In-Practice/Text-Rendering
	ImGui Documentatation - 
	Open File using Window API Tutorial - 


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
#pragma comment(lib, "freetype.lib")

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
#include <algorithm>
#include <map>
#include <windows.h>
#include <commdlg.h>


/* Tutorial used for installing freetype library https://www.youtube.com/watch?v=qW_8Dyq2asc */
#include <ft2build.h>
#include FT_FREETYPE_H

#include "axes.h"
#include "quad.h"
#include "cube.h"

/* Include the GLM maths library for GLM functions along with matrix extensions	*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

/* GL unassigned integers used for buffer arrays and shader programs */
GLuint program;
GLuint textureShaders;
GLuint vArrayObj;
GLuint textVertexArrayObj;
GLuint textBufferObject;
GLuint xAxesBufferObject, xColourBuffer;
GLuint yAxesBufferObject, yColourBuffer;
GLuint zAxesBufferObject, zColourBuffer;
GLuint plotBufferObject, plotColourBuffer;

GLuint quadBO;
GLuint quadColourBO;
GLuint quadTextBO;

GLuint modelID1, viewID1, projectionID1, colourModeID1;
GLuint modelID2, viewID2, projectionID2;

bool firstMouse = true;
bool moveScene = false;
float yaw = 0.0f;
float pitch = 0.0f;
float lastX = 1024.f / 2.0;
float lastY = 768.f / 2.0;
float fov = 45.f;
float scaler = 0.5f;

float camY = 0;
float camX = 0;

int size;
bool clear;

float largest;
float labelBoundary;
GLfloat sizePoint;
GLuint colourMode;
static int graphType = 0;
bool drawmode;

float bump;
float addby;

int numberofBarsX;

std::vector<int> barsX;

int numberofBarsZ;

ThreeDAxes newAxes;
Quad newQuad;
Cube testCube;

static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat aspect_ratio;

static const char * graphs[] = { "Scatter Plot", "Line Graph", "Bar Chart"};

static char Xlabel[128] = "";
static char Ylabel[128] = "";
static char Zlabel[128] = "";

std::string labelCheckX;
std::string labelCheckY;
std::string labelCheckZ;

std::vector<float> vertexPos;
std::vector<float> vertexColours = {1.0, 1.0, 1.0, 1.0};

std::vector<std::string> labels;


std::vector<std::string> positiveLabels;
std::vector<std::string> negativeLabels;

std::vector<Quad> numberLables;

std::vector<Quad> XAxesLabel;
std::vector<Quad> YAxesLabel;
std::vector<Quad> ZAxesLabel;

struct Character
{
	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

std::map<char, Character> Characters;


std::vector<float> read3DData(std::string filePath);

void clearGraphVector();

void splitLabelVectors();

void createNumberLabels();

void makeAxesNames();

void createBars();

/*
*  Initialising Function, called before rendering loop to initialise variables and creating objects
*/
void init(GLWrapper* glw)
{
	// define the aspect ratio used in the perspective call in display
	aspect_ratio = 1024.f / 768.f;

	glGenVertexArrays(1, &vArrayObj); // generate index (name) for one vertex array object
	glGenVertexArrays(1, &textVertexArrayObj);

	//glBindVertexArray(vArrayObj); // create the vertex array object and make it current

	// try to load in both vertex and fragment shaders
	try
	{
		// set program variable to loaded shaders
		program = glw->LoadShader("../../shaders/shaderVert.vert", "../../shaders/shaderFrag.frag");
		textureShaders = glw->LoadShader("../../shaders/textureVert.vert", "../../shaders/textureFrag.frag");
	}
	catch (std::exception& e) // catch any exceptions
	{
		// print exception found
		std::cout << "Exception found: " << e.what() << std::endl;

		// ignore any input
		std::cin.ignore();
		exit(0); // exit program
	}

	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Freetype Checks

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init Freetype Library" << std::endl;
	}
	else
	{
		std::cout << "Freetype Library Loaded" << std::endl;
	}


	FT_Face face;
	if (FT_New_Face(ft, "../../fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR:FREETYPE: Failed to load font" << std::endl;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
				continue;
			}

			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			


			Character letter = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)

			};

			Characters.insert(std::pair<char, Character>(c, letter));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// create axes with the largest number from data set.
	
	labels = newAxes.makeAxes(largest);

	splitLabelVectors();

	sizePoint = 10.0f;

	labelCheckX = Xlabel;
	labelCheckY = Ylabel;
	labelCheckZ = Zlabel;

	testCube.makeCube(1, 1);

	numberofBarsX = 0;
	numberofBarsZ = 0;

	//vertexColours.push_back(0);

	//std::cout << numLabels.empty() << std::endl;

	modelID1 = glGetUniformLocation(program, "model");
	colourModeID1 = glGetUniformLocation(program, "colourMode");
	viewID1 = glGetUniformLocation(program, "view");
	projectionID1 = glGetUniformLocation(program, "projection");

	modelID2 = glGetUniformLocation(textureShaders, "model");
	viewID2 = glGetUniformLocation(textureShaders, "view");
	projectionID2 = glGetUniformLocation(textureShaders, "projection");

}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glBindVertexArray(vArrayObj);

	glUseProgram(program);

	std::stack<glm::mat4> model;

	model.push(glm::mat4(1.0));

	glm::mat4 projection3D = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);

	//glm::mat4 projection2D = glm::ortho(0.0f, 800.f, 0.0f, 600.0f);

	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 0, 4),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	if (largest < 9)
	{
		bump = 1;
		addby = 1;
	}
	else
	{
		bump = 2;
		addby = 2;
	}

	//view = glm::rotate(view, -glm::radians(yaw), glm::vec3(1, 0, 0));
	//view = glm::rotate(view, glm::radians(20.0f), glm::vec3(1, 0, 0));

	//model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
	//model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniform1ui(colourModeID1, colourMode);
	glUniformMatrix4fv(viewID1, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID1, 1, GL_FALSE, &projection3D[0][0]);

	model.top() = glm::scale(model.top(), glm::vec3(scaler, scaler, scaler));
	model.top() = glm::translate(model.top(), glm::vec3(camX, camY, 0));

	// draws axes
	model.push(model.top());
	{

		//model.top() = glm::scale(model.top(), glm::vec3(1, 1, 1));

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));
		

		glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		newAxes.drawAxes();

	}
	model.pop();


	// draws graphs
	model.push(model.top());
	{

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));

	
		glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // modify 3 to change between 2d and 3d

		glGenBuffers(1, &plotColourBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexColours.size() * sizeof(float), &(vertexColours[0]), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		//testCube.makeCube(1, 1);
		//testCube.drawCube(0);

		glPointSize(sizePoint);

		if (graphType == 0)
		{
			if (size / 3 >= 1)
			{
				glDrawArrays(GL_POINTS, 0, size / 3);
			}
			else if (size / 2)
			{
				glDrawArrays(GL_POINTS, 0, size / 2);
			}
		}
		else if (graphType == 1) 
		{
			if (size / 3 >= 1)
			{
				if (!drawmode)
				{
					glDrawArrays(GL_LINE_STRIP, 0, size / 3);
				}
				else
				{
					glDrawArrays(GL_LINE_LOOP, 0, size / 3);
				}
			}
			else if (size / 2)
			{
				if (!drawmode)
				{
					glDrawArrays(GL_LINE_STRIP, 0, size / 3);
				}
				else
				{
					glDrawArrays(GL_LINE_LOOP, 0, size / 3);
				}
			}
		}
		else if (graphType == 2) 
		{
			if (!(vertexPos.size() == 1 && vertexPos[0] == 0))
			{
				for (int i = 0; i < vertexPos.size(); i++)
				{
					testCube.makeCube(vertexPos[i], i);
					testCube.drawCube(0);
				}
			}
		}

	}
	model.pop();

	model.push(model.top()); 
	{
		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));

		model.top() = glm::translate(model.top(), glm::vec3(0.2,0.25,0.2));
		model.top() = glm::scale(model.top(), glm::vec3(0.25, 1, 0.25));


		glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		testCube.drawCube(0);

		// apply transformations
		// sned uniform var
		// draw cube

	}
	model.pop();

	glUseProgram(0);
	glUseProgram(textureShaders);

	glUniformMatrix4fv(viewID2, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID2, 1, GL_FALSE, &projection3D[0][0]);

	// draw the name for the X axes
	model.push(model.top());
	{

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));

		model.top() = glm::translate(model.top(), glm::vec3(0, -0.2, 0));

		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		std::string::const_iterator q = labelCheckX.begin();

		if (!XAxesLabel.empty())
		{
			for (int x = 0; x < XAxesLabel.size(); x++)
			{
				Character label = Characters[*q];

				glActiveTexture(GL_TEXTURE0);

				glBindVertexArray(textVertexArrayObj);

				int loc = glGetUniformLocation(textureShaders, "text");
				if (loc >= 0) glUniform1i(loc, 0);

				glBindTexture(GL_TEXTURE_2D, label.TextureID);

				XAxesLabel[x].drawQuad();

				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				q++;
			}
		}


	}
	model.pop();

	// draw the name for the Y axes
	model.push(model.top());
	{

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));

		model.top() = glm::translate(model.top(), glm::vec3(-0.2, -0.2, 0));


		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		//model.top() = glm::translate(model.top(), glm::vec3(-1, 0, 0));

		std::string::const_iterator q = labelCheckY.begin();

		if (!YAxesLabel.empty())
		{
			for (int y = 0; y < YAxesLabel.size(); y++)
			{
				Character label = Characters[*q];

				glActiveTexture(GL_TEXTURE0);

				glBindVertexArray(textVertexArrayObj);

				int loc = glGetUniformLocation(textureShaders, "text");
				if (loc >= 0) glUniform1i(loc, 0);

				glBindTexture(GL_TEXTURE_2D, label.TextureID);

				YAxesLabel[y].drawQuad();

				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				q++;
			}
		}
	}
	model.pop();

	// draw the name for the z axes
	model.push(model.top());
	{

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));

		model.top() = glm::translate(model.top(), glm::vec3(0, -0.2, 0.01));


		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		std::string::const_iterator q = labelCheckZ.begin();

		if (!ZAxesLabel.empty())
		{
			for (int z = 0; z < ZAxesLabel.size(); z++)
			{
				Character label = Characters[*q];

				glActiveTexture(GL_TEXTURE0);

				glBindVertexArray(textVertexArrayObj);

				int loc = glGetUniformLocation(textureShaders, "text");
				if (loc >= 0) glUniform1i(loc, 0);

				glBindTexture(GL_TEXTURE_2D, label.TextureID);

				ZAxesLabel[z].drawQuad();

				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				q++;

			}
		}
	}
	model.pop();

	// draws labels (numbers)
	model.push(model.top());
	{

		model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f));
		model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f));

		model.top() = glm::translate(model.top(), glm::vec3(0, -0.4, 0));

		//model.top() = glm::rotate(model.top(), glm::radians(180.f), glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		// draw labels from quad vector

		int track = 0;
		std::string::const_iterator q;

		while (track < numberLables.size())
		{

			for (int i = 0; i < positiveLabels.size(); i++)
			{
				if (!numberLables.empty())
				{
					q = positiveLabels[i].begin();

					for (q = positiveLabels[i].begin(); q != positiveLabels[i].end(); q++)
					{

						Character texTEST = Characters[*q];

						glActiveTexture(GL_TEXTURE0);

						glBindVertexArray(textVertexArrayObj);

						int loc = glGetUniformLocation(textureShaders, "text");
						if (loc >= 0) glUniform1i(loc, 0);

						glBindTexture(GL_TEXTURE_2D, texTEST.TextureID);

						numberLables[track].drawQuad();
						track++;

						glBindVertexArray(0);
						glBindTexture(GL_TEXTURE_2D, 0);
					}

				}
			}
			for (int i = 0; i < negativeLabels.size(); i++)
			{
				if (!numberLables.empty())
				{
					for (q = negativeLabels[i].begin(); q != negativeLabels[i].end(); q++)
					{
						Character texTEST = Characters[*q];

						glActiveTexture(GL_TEXTURE0);

						glBindVertexArray(textVertexArrayObj);

						int loc = glGetUniformLocation(textureShaders, "text");
						if (loc >= 0) glUniform1i(loc, 0);

						glBindTexture(GL_TEXTURE_2D, texTEST.TextureID);

						numberLables[track].drawQuad();
						track++;


						glBindVertexArray(0);
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}
			}
		}

	}
	model.pop();

	// IMGUI CODE

	ImGui::Begin("MULTIDIMENSIONAL PLOTTER");
	
		ImGui::Text("Welcome to Multidimensional Plotter");

		ImGui::Dummy(ImVec2(0.0f, 10.f));

		// Open a file to read in using windows.h api
		if (ImGui::Button("Open File")) 
		{
			// https://www.youtube.com/watch?v=-iMGhSlvIR0
			// https://docs.microsoft.com/en-us/answers/questions/483237/a-value-of-type-34const-char-34-cannot-be-assigned.html

			OPENFILENAME ofn;

			wchar_t file_name[MAX_PATH];
			const wchar_t spec[] = L"Text Files\0*.TXT\0";

			ZeroMemory(&ofn, sizeof(OPENFILENAME));

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = GetFocus();
			ofn.lpstrFile = file_name;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = spec;
			ofn.nFilterIndex = 1;
			
			GetOpenFileName(&ofn);

			std::wstring convert(ofn.lpstrFile);

			std::string path(convert.begin(), convert.end());

			std::cout << path << std::endl;

			if (!path.empty())
			{
				vertexPos = read3DData(path);

				size = vertexPos.size();

				// clear labels...
				for (int i = 0; i < numberLables.size(); i++)
				{
					numberLables[i].clearQuad();
				}
				numberLables.clear();
				positiveLabels.clear();
				negativeLabels.clear();

				if (graphType == 2)
				{
					createBars();
				}

				labels = newAxes.makeAxes(largest);

				splitLabelVectors();

				createNumberLabels();

				makeAxesNames();
				
				glGenBuffers(1, &plotBufferObject);
				glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
				glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &(vertexPos[0]), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

		}

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		if (ImGui::Combo("Graph", &graphType, graphs, IM_ARRAYSIZE(graphs))) 
		{
			clearGraphVector();
			newAxes.clearLabels();
			for (int i = 0; i < numberLables.size(); i++)
			{
				numberLables[i].clearQuad();
			}
			numberLables.clear();
			positiveLabels.clear();
			negativeLabels.clear();
		}

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		if (graphType == 0) {

			ImGui::SliderFloat("Point Size", &sizePoint, 5.0f, 20.f);
			ImGui::Dummy(ImVec2(0.0f, 5.f));
		}

		if (graphType == 1) {
			ImGui::Checkbox("Line Loop", &drawmode);
			ImGui::Dummy(ImVec2(0.0f, 5.f));
		}

		ImGui::SliderFloat("Graph Size", &scaler, 0.01f, 1.f);
		ImGui::Dummy(ImVec2(0.0f, 5.f));

		ImGui::InputTextWithHint("X Axes", "INSERT NAME HERE", Xlabel, IM_ARRAYSIZE(Xlabel));

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		ImGui::InputTextWithHint("Y Axes", "INSERT NAME HERE", Ylabel, IM_ARRAYSIZE(Ylabel));

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		ImGui::InputTextWithHint("Z Axes", "INSERT NAME HERE", Zlabel, IM_ARRAYSIZE(Zlabel));

		if (labelCheckX != Xlabel || labelCheckY != Ylabel || labelCheckZ != Zlabel)
		{
			ImGui::Dummy(ImVec2(0.0f, 3.f));
			if (ImGui::Button("UPDATE LABELS"))
			{
				labelCheckX = Xlabel; // update the label and make quads regarding that
				labelCheckY = Ylabel; // update the label and make quads regarding that
				labelCheckZ = Zlabel; // update the label and make quads regarding that
				makeAxesNames();
				// make new quads for this label
			}

		}

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		if (ImGui::Button("RESET POSITION"))
		{
			camX = 0;
			camY = 0;
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear Graph"))
		{
			clearGraphVector();
			newAxes.clearLabels();

			for (int i = 0; i < numberLables.size(); i++)
			{
				numberLables[i].clearQuad();
			}

			numberLables.clear();
			positiveLabels.clear();
			negativeLabels.clear();
		}

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		ImGui::Text("Data Point Colour");

		ImGui::Dummy(ImVec2(0.0f, 5.f));

		if (ImGui::ColorEdit3(" ", color)) 
		{
			if (!vertexPos.empty())
			{
				vertexColours.clear();

				for (int i = 0; i < vertexPos.size(); i++)
				{
					vertexColours.insert(vertexColours.end(), { color[0] , color[1] , color[2] , color[3] });
				}
			}
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
	if (key == GLFW_KEY_UP)
	{
		camY -= 0.1f;
	}
	else if (key == GLFW_KEY_DOWN)
	{
		camY += 0.1f;
	}
	else if (key == GLFW_KEY_RIGHT)
	{
		camX -= 0.1f;
	}
	else if (key == GLFW_KEY_LEFT)
	{
		camX += 0.1f;
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
*	Mouse button callback aquired from GLFW help page https://www.glfw.org/docs/3.3/input_guide.html
*/
static void mouseButonCallback(GLFWwindow* window, int button, int action, int mods) 
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE || button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{
			moveScene = true;
			firstMouse = true;
		}
		else if(action == GLFW_RELEASE)
		{
			moveScene = false;
		}

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
std::vector<float> read3DData(std::string filePath)
{
	std::vector<std::string> vertexPositions;
	bool invalid = false;
	numberofBarsZ = 0;

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

		numberofBarsZ = 0;


		while (ss >> num)
		{

			std::cout << num << std::endl;

			for (int i = 0; i < num.length(); i++)
			{
				if (isalpha(num[i]))
				{
					invalid = true;
					break;
				}
			}
			
			if (invalid)
			{
				std::cout << "INVALID DATA LINE - REMOVING" << std::endl;
				invalid = false;
			}
			else
			{
				plotPos.push_back(std::stof(num));

				if (std::stof(num) > largest)
				{
					largest = std::stof(num);
				}
				else if(-(std::stof(num)) > largest)
				{
					largest = -std::stof(num);
				}
			}
			numberofBarsZ++;

		}
		barsX.insert(barsX.end(), numberofBarsZ);

	}

	//std::cout << "NUM: " << numberofBarsZ/3 << std::endl;

	for (int i = 0; i < barsX.size(); i++)
	{
		std::cout << barsX[i] << std::endl;
	}

	// if graph type == 2
	// perform bar chart things

	if (largest < 10)
	{
		bump = 1;
		addby = 1;
	}
	else
	{
		bump = 2;
		addby = 2;
	}

	if (!vertexColours.empty())
	{
		vertexColours.clear();
	}

	for (int i = 0; i < plotPos.size(); i++)
	{
		vertexColours.insert(vertexColours.end(), { color[0] , color[1] , color[2] , color[3] });
	}

	if (plotPos.empty())
	{
		labelBoundary = largest;
		std::vector<float> check;
		check.push_back(0);
		return check;
	}
	else
	{
		labelBoundary = largest;
		return plotPos;
	}
}

/*
* Clears the plot vector and creates new buffers to clear data on screen
*/
void clearGraphVector() 
{
	labelBoundary = largest;

	std::cout << "CLEAR THE GRAPH" << std::endl;
	vertexPos.clear();
	vertexPos.push_back(0);

	size = vertexPos.size();

	largest = 0;

	glGenBuffers(1, &plotBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &(vertexPos[0]), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void splitLabelVectors() 
{
	for (int i = 0; i < labels.size(); i++)
	{
		if (!labels.empty())
		{
			if (std::stof(labels[i]) < 0)
			{
				negativeLabels.push_back(std::to_string(std::stoi(labels[i])));
			}
			else
			{
				positiveLabels.push_back(std::to_string(std::stoi(labels[i])));
			}
		}
	}

	std::reverse(negativeLabels.begin(), negativeLabels.end());
}


/*
* Creates Quads to fit number and negative symbol. Quads are added to quad vector to be drawn in render loop.
* Old Method was problem since this would be called in render loop, leaking memory as each quad was made.
*/
void createNumberLabels() 
{
	float yBump = 0;
	int count = 0;
	int bar = 0;
	int limit = 3;
	std::string::const_iterator test;

	if (graphType == 2)
	{
		bar = 1;
		limit = 2;

	}


	std::string jim;
	float temp = bump;

	for (int j = bar; j < limit; j++)
	{
		std::cout << bump << std::endl;

		for (int i = 0; i < positiveLabels.size(); i++)
		{
			float cycle = 0;

			if (!labels.empty())
			{
				jim = positiveLabels[i];

				yBump = 0;

				for (test = jim.begin(); test != jim.end(); test++)
				{
					if (j == 1)
					{
						if (cycle >= 1)
						{
							yBump = yBump + 0.1f;
						}
					}
					else
					{
						if (cycle >= 1 && j != 1) 
						{
							bump = bump + 0.1f;
						}
					}

					numberLables.insert(numberLables.end(), newQuad);
					numberLables[count].makeQuad(bump, 1, j, yBump);

					count++;

					cycle = cycle + 1;

				}

				if (cycle > 1 && j != 1)
				{
					bump = bump - (0.1f * (cycle - 1));
				}

				yBump = 0;

				bump = bump + addby;

			}

		}
	
		bump = bump - addby;

		for (int i = 0; i < negativeLabels.size(); i++)
		{
			float cycle2 = 0;

			if (!labels.empty())
			{
				jim = negativeLabels[i];

				yBump = 0;

				for (test = jim.begin(); test != jim.end(); test++)
				{
					if (j == 1)
					{
						if (cycle2 >= 1)
						{
							yBump = yBump + 0.1f;
						}
					}
					else
					{
						if (cycle2 >= 1 && j != 1)
						{
							bump = bump - 0.1f;
						}
					}

					numberLables.insert(numberLables.end(), newQuad);

					if (cycle2 == 0)
					{
						numberLables[count].makeQuad(-(bump), 0, j, yBump);
					}
					else
					{
						numberLables[count].makeQuad(-(bump), 1, j, yBump);
					}


					count++;

					cycle2 = cycle2 + 1;

				}

				if (cycle2 > 1 && j!= 1)
				{
					bump = bump + (0.1f * (cycle2 - 1));
				}

				yBump = 0;

				bump = bump - addby;

			}
		}
		bump = temp;

	}

}

void makeAxesNames() 
{
	if (!XAxesLabel.empty() || !YAxesLabel.empty() || !ZAxesLabel.empty())
	{

		for (int x = 0; x < XAxesLabel.size(); x++)
		{
			XAxesLabel[x].clearQuad();
		}
		for (int y = 0; y < YAxesLabel.size(); y++)
		{
			YAxesLabel[y].clearQuad();
		}
		for (int z = 0; z < ZAxesLabel.size(); z++)
		{
			ZAxesLabel[z].clearQuad();
		}

		XAxesLabel.clear();
		YAxesLabel.clear();
		ZAxesLabel.clear();
	}

	std::string::const_iterator t;
	int loop = 0;
	float indent = labelBoundary+1.1f;



	for (t = labelCheckX.begin(); t != labelCheckX.end() ; t++)
	{
		labelCheckX[loop] = std::toupper(labelCheckX[loop]);
		if (labelCheckX[loop] == 'I' )
		{
			XAxesLabel.insert(XAxesLabel.end(), newQuad);
			XAxesLabel[loop].makeQuad(indent, 2, 0, 0);
			loop++;
			indent = indent + 0.1f;
		}
		else
		{
			XAxesLabel.insert(XAxesLabel.end(), newQuad);
			XAxesLabel[loop].makeQuad(indent, 1, 0, 0);
			loop++;
			indent = indent + 0.1f;
		}
	}

	loop = 0;
	float yIndent = 0.1f;
	indent = labelBoundary + 1.1f;

	for (t = labelCheckY.begin(); t != labelCheckY.end(); t++)
	{
		labelCheckY[loop] = std::toupper(labelCheckY[loop]);
		if (labelCheckY[loop] == 'I')
		{
			YAxesLabel.insert(YAxesLabel.end(), newQuad);
			YAxesLabel[loop].makeQuad(indent, 2, 1, yIndent);
			loop++;
			yIndent = yIndent + 0.1f;
		}
		else
		{
			labelCheckY[loop] = std::toupper(labelCheckY[loop]);
			YAxesLabel.insert(YAxesLabel.end(), newQuad);
			YAxesLabel[loop].makeQuad(indent, 1, 1, yIndent);
			loop++;
			yIndent = yIndent + 0.1f;
		}
	}


	loop = 0;
	indent = labelBoundary + 1.1f;

	for (t = labelCheckZ.begin(); t != labelCheckZ.end(); t++)
	{
		labelCheckZ[loop] = std::toupper(labelCheckZ[loop]);
		if (labelCheckY[loop] == 'I')
		{
			ZAxesLabel.insert(ZAxesLabel.end(), newQuad);
			ZAxesLabel[loop].makeQuad(indent, 2, 2, 0);
			loop++;
			indent = indent + 0.1f;
		}
		else
		{
			labelCheckZ[loop] = std::toupper(labelCheckZ[loop]);
			ZAxesLabel.insert(ZAxesLabel.end(), newQuad);
			ZAxesLabel[loop].makeQuad(indent, 1, 2, 0);
			loop++;
			indent = indent + 0.1f;
		}
	}

	// loop through each label string (Starting with X)
	// insert a new quad object into the quad vector the quads belong to i.e. x axes name
	// create a quad for each character in that string

}

void createBars() 
{

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
	glw->setMouseButtonCallback(mouseButonCallback);
	glw->setScrollCallback(scrollCallback);
	glw->setReshapeCallback(reshape);
	glw->DisplayVersion();

	init(glw);

	glw->eventLoop();


	delete(glw);
	return 0;
}
