/*
	Max Fyall - 180011724
	Multidimensional Plotter

	Application Code
*/

/*	
	REFERENCES USED:

	[1] Freetype Code Reference - https://learnopengl.com/In-Practice/Text-Rendering
	[2] Freetype Library - https://freetype.org/
	[3] ImGui Documentatation - https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html
	[4] ImGui Setup Guide - https://www.youtube.com/watch?v=VRwhNKoxUtk
	[5] Open File using Window API Tutorial - https://www.youtube.com/watch?v=-iMGhSlvIR0
	[6] Microsoft Documentation - https://docs.microsoft.com/en-us/answers/questions/483237/a-value-of-type-34const-char-34-cannot-be-assigned.html
	[7] GLFW Mouse Button Callback - https://www.glfw.org/docs/3.3/input_guide.html
	[8] GLFW Mouse Camera Control - https://learnopengl.com/Getting-started/Camera
	[9] Split a String C++ - https://www.geeksforgeeks.org/how-to-split-a-string-in-cc-python-and-java/
	[10] Freetpye Installation Guide - https://www.youtube.com/watch?v=qW_8Dyq2asc

	NOTE: REALISATION - INCORRECT SPELLING OF AXIS IS USED A LOT. MOST OF THE TIME IT IS SPELT AXES INSTEAD OF AXIS. APOLOGIES FOR ANY CONFUSION

*/

#define UNICODE
#pragma comment(lib, "user32.lib")

/* Static Library Links - DEBUG/RELEASE MODE */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glloadD.lib")
#else
#define GLEW_STATIC // GLEW_Static require for running GLEW in static mode
#pragma comment(lib, "glfw3_mt.lib")
#pragma comment(lib, "glew32s.lib")
#endif
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "freetype.lib")

/* Include GLFW wrapper class (Written by Dr. Iain Martin) for GLFW and OpenGL extensions */
#include "wrapper_glfw.h"

/* Include standard C++ libraries for help in functions */
#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <windows.h>
#include <commdlg.h>

/* Tutorial used for installing freetype library (SEE REFERENCE [10]) */
#include <ft2build.h>
#include FT_FREETYPE_H

/* Include Header Files of required classes */
#include "axis.h"
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

/* Unassigned integers used for locations of uniform matricies passed to relevent vertex shaders*/
GLuint modelID1, viewID1, projectionID1;
GLuint modelID2, viewID2, projectionID2;

/* Scene control variables*/
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
float camZ = 0;

int size; // check up on this

/* Graphing Variables */

float largest; // holds the largest value from loaded data set
float labelBoundary; // determines where axes names appear
GLfloat sizePoint; // holds size of points (edited by ImGUI)
static int graphType = 0; // integer var determines which graph to draw
bool drawmode; // used in line graph (determines if line graph points are connected)

bool twoD = false; // sets drawing mode to 2D
bool threeD = true; // sets drawing mode to 3D (Set to true by default)

/* Axes Label variables */
float bump; // used to move label to next spot
float addby; // keeps track of how much to move label by

/* Holds number of bars to make in X direction */
std::vector<int> barsX;

/* Class Objects */
ThreeDAxis newAxes; // Axes class instance, draws the X, Y, Z axes
Quad newQuad; // Quad class instance, used to make and draw quads for labels
Cube testCube; // Cube class instance, used to make and draw cubes (rectangles for bar chart)

/* Colour Float, allows user to change colour of data points */
static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

/* Aspect ration var - stores the aspect res of the GLFW Window */
GLfloat aspect_ratio;

/* Char Array used in ImGUI */
static const char * graphs[] = { "Scatter Plot", "Line Graph", "Bar Chart"};

/* Char's for Axes Names (Edited by ImGUI)*/
static char Xlabel[128] = "";
static char Ylabel[128] = "";
static char Zlabel[128] = "";

/* Strings to used to store current label */
std::string labelCheckX;
std::string labelCheckY;
std::string labelCheckZ;

/* Data point Vectors */
std::vector<float> vertexPos; // stores vertex positions (read in from file)
std::vector<float> vertexColours = {1.0, 1.0, 1.0, 1.0}; // stores colour values for data points

/* Vectors for storing number labels */
std::vector<std::string> labels; // stores all labels generated from axes class

/* Seperate vectors for positive and negative labels */
std::vector<std::string> positiveLabels;
std::vector<std::string> negativeLabels;


/* Vector of Quads for number labels */
std::vector<Quad> numberLables;

/* Vector of Quads for Axes Names */
std::vector<Quad> XAxesLabel;
std::vector<Quad> YAxesLabel;
std::vector<Quad> ZAxesLabel;

/* Vector of Cubes for bar chart - Holds neccessary number of cubes (rectangles) */
std::vector<Cube> barChart;

/* Character Struct - Holds font data (acquired from the LearnOpenGL Tutorial */
struct Character
{
	unsigned int TextureID; // texture id - distinguishes textures

	/* Other variables not used */
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

/* Map for key value pairs (key - char ('y') value - Character Struct)*/
std::map<char, Character> Characters;


/* Function Declarations */

std::vector<float> readData(std::string filePath); // read from file method

void clearGraphVector(); // clears the vector positions vector (removing data from the screen)

void splitLabelVectors(); // splits the label vector into the positive/negative vectors

void createNumberLabels(); // creates the quads for the number variables

void makeAxesNames(); // creates quads for the axes name

void createBars(); // creates cubes for bar chart

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

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* FREETYPE LIBRARY INITIALISING - AQUIRED FROM LEARNOPENGL - (SEE REFERENCE [1] and [2]) */

	// Freetype Checks

	FT_Library ft; // create freetype instance
	if (FT_Init_FreeType(&ft)) // check if lib can be initialised
	{
		std::cout << "ERROR::FREETYPE: Could not init Freetype Library" << std::endl; // display error
	}
	else
	{
		std::cout << "Freetype Library Loaded" << std::endl; // confirmation
	}


	FT_Face face; // create font face
	if (FT_New_Face(ft, "../../fonts/arial.ttf", 0, &face)) // create new face from font file (returns error code if fails)
	{
		std::cout << "ERROR:FREETYPE: Failed to load font" << std::endl; // return error
	}
	else
	{

		// set size to load glyphs
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 chracters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
				continue;
			}

			// Generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glGenerateMipmap(GL_TEXTURE_2D); // Generate Mipmaps - Limit some aliasing issues (Unsure if this has any effect due to the sizes)

			// set texture options - using mipmaps
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			

			// store the character for later use
			Character letter = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)

			};

			// add char and character struct to map
			Characters.insert(std::pair<char, Character>(c, letter));
		}
		glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
	}
	
	// destroy Freetype once finished (Prevent Unecessary memory usage)
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// create axes with the largest number from data set.
	labels = newAxes.makeAxis(largest, 0); // make axes returns vector of floats containing the labels required for x y z axes

	splitLabelVectors(); // split "labels" vector into positive and negative vectors

	sizePoint = 10.0f; // initialise points size

	// initialise axes name variables, set them to static chars that are change by ImGUI input box
	labelCheckX = Xlabel;
	labelCheckY = Ylabel;
	labelCheckZ = Zlabel;

	// uniform location variables for first shader
	modelID1 = glGetUniformLocation(program, "model");
	viewID1 = glGetUniformLocation(program, "view");
	projectionID1 = glGetUniformLocation(program, "projection");

	// uniform location variables for second shader (used for texturing labels)
	modelID2 = glGetUniformLocation(textureShaders, "model");
	viewID2 = glGetUniformLocation(textureShaders, "view");
	projectionID2 = glGetUniformLocation(textureShaders, "projection");

	/* Create Colour Buffer for data points */
	glGenBuffers(1, &plotColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexColours.size() * sizeof(float), &(vertexColours[0]), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

/*
* Display Function - The main render loop
* - In main we tell our GLFW instance this is our render loop
* - GLFW class will use this function in the event loop function inside of the GLFW Class
*/
void display()
{
	// set background colour of scene
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// clear colour and frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable to depth test
	glEnable(GL_DEPTH_TEST);

	// make one of our vertex array objects current (i.e. this VAO is used for everything minus the labels)
	glBindVertexArray(vArrayObj);

	// bind our first shader program i.e. make it current
	glUseProgram(program);

	// create a stack of matrices (used in transformations)
	std::stack<glm::mat4> model;

	// push an indentity matrix onto the top of the stack
	model.push(glm::mat4(1.0));

	// create projection matrix - 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projection3D = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);

	// view matrix (Camera Matrix) - 
	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
		glm::vec3(0, 0, 0),	// and looks at the origin
		glm::vec3(0, 1, 0)	// Head is up (set to 0,-1,0 to look upside-down)
	);

	// checks for updating label bump variables (corresponds to marks on each axes)
	if (graphType == 2)
	{
		bump = 1;
		addby = 1;
	}
	else
	{
		if (largest < 10)
		{
			// set label variables to one (since distance between each axes mark is 1)
			bump = 1;
			addby = 1;
		}
		else
		{
			// set label variables to two (since distance between each axes mark is 2)
			bump = 2;
			addby = 2;
		}
	}

	// send the uniform matricies to the vertex shader (used in model-view-projection calculation)
	glUniformMatrix4fv(viewID1, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID1, 1, GL_FALSE, &projection3D[0][0]);

	// apply transformations to the whole scene
	model.top() = glm::scale(model.top(), glm::vec3(scaler, scaler, scaler)); // scale the scene using the variable editted by ImGui
	model.top() = glm::translate(model.top(), glm::vec3(camX, camY, camZ)); // apply a translate using variable update by the user (using the arrow keys)

	/* SCENE ROTATIONS */
	model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f)); // mouse rotation X
	model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f)); // mouse rotation Y

	/* DRAWS THE AXES */
	model.push(model.top()); // push an indentity matrix onto the stack apply transformations to drawn items within
	{	
		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		// draw the 3D axes using the axes class instance
		newAxes.drawAxis();

	}
	model.pop(); // pop from the stack


	// draws graphs (scatter plot and line graphs)
	model.push(model.top());
	{
		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		/* Bind the data verticies read in from file */
		glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
		glEnableVertexAttribArray(0);

		// check for 2D or 3D drawing (FOR SCATTER PLOT ONLY)
		if (graphType == 0)
		{
			if (twoD)
			{
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // specify 2 components
			}
			else if (threeD)
			{
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // specify 3 components
			}
		}
		else // else we want to draw a line plot
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // specify 3 components
		}

		/* Bind the Colour Buffer */
		glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
		glEnableVertexAttribArray(1); // colour in vertex attribute array in position 1
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		// set the point size to updatable variable
		glPointSize(sizePoint);

		// check if graph type is scatter plot
		if (graphType == 0)
		{
			glDrawArrays(GL_POINTS, 0, size); // draw data as points in 2-D/3-D space
		}
		else if (graphType == 1) // check if graph type is line graph
		{
			if (!drawmode) // drawmode represents how the line graph is drawn
			{
				glDrawArrays(GL_LINE_STRIP, 0, size); // use drawing mode LINE_STRIP draw the points as a line
			}
			else
			{
				glDrawArrays(GL_LINE_LOOP, 0, size); // use drawing mode LINE_LOOP draw the points as interconnected line
			}
		}

	}
	model.pop();

	// draw bar chart (when required)
	model.push(model.top()); 
	{
		// scale modifier to apply to the cubes so they look like rectangles
		model.top() = glm::scale(model.top(), glm::vec3(0.25, 1, 0.25));

		// send model uniform matrix to the vertex shader with applied transformations
	    glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		int p = 0; // counter variable to cycle through the vector of cubes

		// check if graph type is bar chart
		if (graphType == 2)
		{	
			// prohibit draw the chart if the cube vector is empty (will cause an exception otherwise)
			if (!barChart.empty())
			{
				// loop for vector size (vector size is equilvalent to the number of bars in the z direction)
				for (int i = 0; i < barsX.size(); i++)
				{
					// loop for integer item in barX vector (each item in this vector is the number of bars in the X direction)
					for (int j = 0; j < barsX[i]; j++)
					{
						barChart[p].drawCube(0); // draw pre-made cube from cube vector
						p++; // increment by one to move to the next cube
					}
				}
			}

		}

	}
	model.pop();

	// unbind the current shader program
	glUseProgram(0);

	// bind our second shader program
	glUseProgram(textureShaders);

	// send the uniform matricies to the vertex shader (used in model-view-projection calculation)
	glUniformMatrix4fv(viewID2, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID2, 1, GL_FALSE, &projection3D[0][0]);

	/* DRAW THE NAME FOR THE X AXES */
	model.push(model.top());
	{
		// transformation to place the quad in the middle of the X axes
		model.top() = glm::translate(model.top(), glm::vec3(0, -0.2, 0));

		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		// string iterator to go throught the Name string (Set it to the first char of the X Axes string)
		std::string::const_iterator q = labelCheckX.begin();

		// stop any drawing if the Quad vector is empty (can cause an exception if not present)
		if (!XAxesLabel.empty())
		{
			// loop through the vector of quads (X axes)
			for (int x = 0; x < XAxesLabel.size(); x++)
			{	
				/* TEXTURING QUADS WITH FREETYPE LIB - SEE REFERENCE[2] */
				
				// create a character struct using the characters map to identify which character to use
				// variable label is a character struct, it is assigned values by setting it to an instance in the characters map using the string iterator as a pointer
				// this pointer will be used as a key and the pairing character struct will be set to label.
				// the main purpose of this is to acquire the character texture required at the current time
				Character label = Characters[*q];

				// make textures active
				glActiveTexture(GL_TEXTURE0);

				// make our other VAO current
				glBindVertexArray(textVertexArrayObj);

				// This is the location of the texture object (TEXTURE0), i.e. tex1 will be the name
				// of the sampler in the fragment shader
				// Standard bit of code to enable a uniform sampler for our texture 
				int loc = glGetUniformLocation(textureShaders, "text");
				if (loc >= 0) glUniform1i(loc, 0);

				// Bind the texture (character) using the texture ID variable inside the label struct
				glBindTexture(GL_TEXTURE_2D, label.TextureID);

				// draw the quad to texture the character on to
				XAxesLabel[x].drawQuad();

				// unbind the VAO and the texture (unbinding the texture will prevent it from texturing anything else)
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				// move the iterator onto the next character in the label string
				q++;
			}
		}


	}
	model.pop();

	/* DRAW THE NAME FOR THE Y AXES */
	model.push(model.top());
	{
		// transform the Y label so it starts in the middle of the Y Axes
		model.top() = glm::translate(model.top(), glm::vec3(-0.2, -0.2, 0));

		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		// string iterator to go throught the Name string (Set it to the first char of the Y Axes string)
		std::string::const_iterator q = labelCheckY.begin();

		// stop any drawing if the Quad vector is empty (can cause an exception if not present)
		if (!YAxesLabel.empty())
		{
			// loop through the vector of quads (Y axes)
			for (int y = 0; y < YAxesLabel.size(); y++)
			{
				/* TEXTURING QUADS WITH FREETYPE LIB - SEE REFERENCE[2] */

				// create a character struct using the characters map to identify which character to use
				// variable label is a character struct, it is assigned values by setting it to an instance in the characters map using the string iterator as a pointer
				// this pointer will be used as a key and the pairing character struct will be set to label.
				// the main purpose of this is to acquire the character texture required at the current time
				Character label = Characters[*q];

				// make textures active
				glActiveTexture(GL_TEXTURE0);

				// make our other VAO current
				glBindVertexArray(textVertexArrayObj);

				// This is the location of the texture object (TEXTURE0), i.e. tex1 will be the name
				// of the sampler in the fragment shader
				// Standard bit of code to enable a uniform sampler for our texture 
				int loc = glGetUniformLocation(textureShaders, "text");
				if (loc >= 0) glUniform1i(loc, 0);

				// Bind the texture (character) using the texture ID variable inside the label struct
				glBindTexture(GL_TEXTURE_2D, label.TextureID);

				// draw the quad to texture the character on to
				YAxesLabel[y].drawQuad();

				// unbind the VAO and the texture (unbinding the texture will prevent it from texturing anything else)
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				// move the iterator onto the next character in the label string
				q++;
			}
		}
	}
	model.pop();

	/* DRAW THE NAME FOR THE Z AXES */
	model.push(model.top());
	{

		// transform the Z label so it starts i the middle of the Z axes
		model.top() = glm::translate(model.top(), glm::vec3(0, -0.2, 0.01));

		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		// string iterator to go throught the Name string (Set it to the first char of the Z Axes string)
		std::string::const_iterator q = labelCheckZ.begin();

		// stop any drawing if the Quad vector is empty (can cause an exception if not present)
		if (!ZAxesLabel.empty())
		{
			// loop through the vector of quads (Z axes)
			for (int z = 0; z < ZAxesLabel.size(); z++)
			{
				/* TEXTURING QUADS WITH FREETYPE LIB - SEE REFERENCE[2] */

				// create a character struct using the characters map to identify which character to use
				// variable label is a character struct, it is assigned values by setting it to an instance in the characters map using the string iterator as a pointer
				// this pointer will be used as a key and the pairing character struct will be set to label.
				// the main purpose of this is to acquire the character texture required at the current time
				Character label = Characters[*q];

				// make textures active
				glActiveTexture(GL_TEXTURE0);

				// make our other VAO current
				glBindVertexArray(textVertexArrayObj);

				// This is the location of the texture object (TEXTURE0), i.e. tex1 will be the name
				// of the sampler in the fragment shader
				// Standard bit of code to enable a uniform sampler for our texture 
				int loc = glGetUniformLocation(textureShaders, "text");
				if (loc >= 0) glUniform1i(loc, 0);

				// Bind the texture (character) using the texture ID variable inside the label struct
				glBindTexture(GL_TEXTURE_2D, label.TextureID);

				// draw the quad to texture the character on to
				ZAxesLabel[z].drawQuad();

				// unbind the VAO and the texture (unbinding the texture will prevent it from texturing anything else)
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				// move the iterator onto the next character in the label string
				q++;

			}
		}
	}
	model.pop();

	/* DRAW THE AXES NUMBERS */
	model.push(model.top());
	{

		// transformation to make numbers appear bellow the notches on the axes
		model.top() = glm::translate(model.top(), glm::vec3(0, -0.4, 0));

		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID2, 1, GL_FALSE, &model.top()[0][0]);

		/* DRAW NUMBERS FROM QUAD VECTOR */

		// integer variable to keep track of number of loop iterations and which character/number to draw
		int track = 0;

		// string iterator
		std::string::const_iterator q;

		// loop for the number of quads inside numberLabels vector
		// track keeps note of number of quads drawn - Quads are made in order of Axes e.g. (X Positve, X Negative, Y Positive etc.)
		// draws quads for positive numbers first then negative numbers for one axes, then process repeats for the remaining axes
		while (track < numberLables.size())
		{
			// first loop - draws positive labels for current axes
			for (int i = 0; i < positiveLabels.size(); i++)
			{
				// check if vector is empty (exception triggered if attempt to access memory that doesn't exist)
				if (!numberLables.empty())
				{

					// loop for the length of the number given i.e. 10
					for (q = positiveLabels[i].begin(); q != positiveLabels[i].end(); q++)
					{
						/* TEXTURING QUADS WITH FREETYPE LIB - SEE REFERENCE[2] */

						// create a character struct using the characters map to identify which character to use
						// variable label is a character struct, it is assigned values by setting it to an instance in the characters map using the string iterator as a pointer
						// this pointer will be used as a key and the pairing character struct will be set to label.
						// the main purpose of this is to acquire the character texture required at the current time e.g. Number = 12, on first iteration we need 1 first then 2
						Character charTex = Characters[*q];

						// make textures active
						glActiveTexture(GL_TEXTURE0);

						// make our other VAO current
						glBindVertexArray(textVertexArrayObj);

						// Standard bit of code to enable a uniform sampler for our texture 
						int loc = glGetUniformLocation(textureShaders, "text");
						if (loc >= 0) glUniform1i(loc, 0);

						// Bind the texture (character) using the texture ID variable inside the charTEX struct
						glBindTexture(GL_TEXTURE_2D, charTex.TextureID);

						// draw the quad to texture the character on to
						numberLables[track].drawQuad();
						track++; // add one to track var (this moves to the next quad in the vector, which is the next char in the string or the next notch number)

						// unbind the VAO and the texture (unbinding the texture will prevent it from texturing anything else)
						glBindVertexArray(0);
						glBindTexture(GL_TEXTURE_2D, 0);
					}

				}
			}
			// second loop - draw negative labels for current axes
			for (int i = 0; i < negativeLabels.size(); i++)
			{
				// check if vector is empty (exception triggered if attempt to access memory that doesn't exist)
				if (!numberLables.empty())
				{

					// loop for the length of the number given i.e. -10
					for (q = negativeLabels[i].begin(); q != negativeLabels[i].end(); q++)
					{
						/* TEXTURING QUADS WITH FREETYPE LIB - SEE REFERENCE[2] */

						// create a character struct using the characters map to identify which character to use
						// variable label is a character struct, it is assigned values by setting it to an instance in the characters map using the string iterator as a pointer
						// this pointer will be used as a key and the pairing character struct will be set to label.
						// the main purpose of this is to acquire the character texture required at the current time e.g. Number = 12, on first iteration we need 1 first then 2
						Character charTex = Characters[*q];

						// make textures active
						glActiveTexture(GL_TEXTURE0);

						// make our other VAO current
						glBindVertexArray(textVertexArrayObj);

						// Standard bit of code to enable a uniform sampler for our texture 
						int loc = glGetUniformLocation(textureShaders, "text");
						if (loc >= 0) glUniform1i(loc, 0);

						// Bind the texture (character) using the texture ID variable inside the charTEX struct
						glBindTexture(GL_TEXTURE_2D, charTex.TextureID);

						// draw the quad to texture the character on to
						numberLables[track].drawQuad();
						track++; // add one to track var (this moves to the next quad in the vector, which is the next char in the string or the next notch number)

						// unbind the VAO and the texture (unbinding the texture will prevent it from texturing anything else)
						glBindVertexArray(0);
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}
			}
		}

	}
	model.pop();


	/* IMGUI CODE - CREATION OF IMGUI FEATURES AND FUNCTIONALITY  (SEE REFERENCE [3], [4]) */

	// Begin - push a window
	ImGui::Begin("MULTIDIMENSIONAL PLOTTER");
	{
		// Tab bar at top of window
		if (ImGui::BeginTabBar("TAB BAR", ImGuiTabBarFlags_None))
		{
			// Menu tab
			if (ImGui::BeginTabItem("HOME"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.f)); // padding

				// Open a file (using file dialog box) to read in using windows.h api (SEE REFERENCE [5], [6])
				if (ImGui::Button("PLOT")) // if button is triggered perform tasks
				{
					// create open file structure
					OPENFILENAME ofn;

					// variables used as elements when setting up ofn struct
					wchar_t file_name[MAX_PATH];
					const wchar_t spec[] = L"Text Files\0*.TXT\0";

					// set all elements to 0
					ZeroMemory(&ofn, sizeof(OPENFILENAME));

					/* Initialising elements of ofn struct */

					ofn.lStructSize = sizeof(OPENFILENAME); // specify size of ofn struct
					ofn.hwndOwner = GetFocus(); // get our window
					ofn.lpstrFile = file_name; // file name set to our wchar_t var file_name
					ofn.lpstrFile[0] = '\0'; // set first char in file name to '\0', mean there is no default file (prevents unexpected results)
					ofn.nMaxFile = MAX_PATH; // set maximum file buffer length of file to be read
					ofn.lpstrFilter = spec; // filter for type of files that can be selected (Right now only text files are allowed but this can be changed to include CSV, JSON etc)
					ofn.nFilterIndex = 1; // which item in filter to select by default, since we have 1 file type we can only select one

					// opens file dialog box and returns file selected in ofn struct
					GetOpenFileName(&ofn);

					// convert the wchar_t to a wstring - to using the file path to read in data
					std::wstring convert(ofn.lpstrFile);

					// another conversion from wstring to string
					std::string path(convert.begin(), convert.end());

					// check if the path is empty i.e. have we read a file in?
					if (!path.empty())
					{

						// clear data from bar chart vectors is there are not empty
						if (!barChart.empty())
						{
							// loop through all the created cubes and clear the vertex buffers to free up some memory
							for (int i = 0; i < barChart.size(); i++)
							{
								barChart[i].clearCube();
							}
							// clear the vectors
							barChart.clear();
							barsX.clear();
						}

						// clear the data point vector (one which stores scatter plot and line plot points)
						vertexPos.clear();

						// set our plotting vector to the return value of read3DData which is a vector of floats
						vertexPos = readData(path);

						// size variable - determines how many points we need to draw
						size = barsX.size(); // set size to number of bars in Z direction since this matches the number of points present in the file

						// clear number labels vector
						for (int i = 0; i < numberLables.size(); i++)
						{
							numberLables[i].clearQuad();
						}

						// clear appropriate vectors
						numberLables.clear();
						positiveLabels.clear();
						negativeLabels.clear();

						// if graph type is bar chart
						if (graphType == 2)
						{
							// call function to create cubes and store them in a vector
							createBars();
							if (largest < barsX.size()) // largest data point may be smaller than the number of bars in a certain direction... if this is the case make largest var this value
							{
								largest = (barsX.size());
								labelBoundary = largest; // set global boundary variable for use in axes name function

							}
						}

						vertexColours.clear(); // clear the colour vector (to add new colours to the vector)

						// insert the RBGA value for every point in the data point vector
						for (int i = 0; i < vertexPos.size(); i++)
						{
							vertexColours.insert(vertexColours.end(), { color[0] , color[1] , color[2] , color[3] }); // vector function insert allows to insert an array of floats (add these to the end of the vector)
						}

						// create our axes and set its return value (vector of strings) to labels vector.
						if (graphType == 2)
						{
							labels = newAxes.makeAxis(largest, 1);

						}
						else 
						{
							if (largest < 10)
							{
								// set label variables to one (since distance between each axes mark is 1)
								bump = 1;
								addby = 1;
							}
							else
							{
								// set label variables to two (since distance between each axes mark is 2)
								bump = 2;
								addby = 2;
							}
							labels = newAxes.makeAxis(largest, 0);
						}						

						// call function to split "labels" vector into two different vectors (Positive and Negative)
						splitLabelVectors();

						// call function to create the quads neccessary for the numbers on the axes... store these in the neccessary vector
						createNumberLabels();

						// call function to create the quads neccessary for the axes names and store the quads in vector
						makeAxesNames();

						/* Create Point Buffer for data points */
						glGenBuffers(1, &plotBufferObject);
						glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
						glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &(vertexPos[0]), GL_DYNAMIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);

						/* Create Colour Buffer for data points */
						glGenBuffers(1, &plotColourBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
						glBufferData(GL_ARRAY_BUFFER, vertexColours.size() * sizeof(float), &(vertexColours[0]), GL_DYNAMIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}

				}

				ImGui::Dummy(ImVec2(0.0f, 5.f)); // padding function

				/* DROP DOWN MENU FOR GRAPH SELECTION */
				if (ImGui::Combo("Graph", &graphType, graphs, IM_ARRAYSIZE(graphs))) // if drop down is selected do the following
				{
					// assumming the user wishes to make a new graph

					// clear data point vector
					clearGraphVector();

					// clear the axes notches
					newAxes.clearLabels();

					// clear the vertex buffers of the number quads
					for (int i = 0; i < numberLables.size(); i++)
					{
						numberLables[i].clearQuad();
					}
					// clear number quad vectors
					numberLables.clear();
					positiveLabels.clear();
					negativeLabels.clear();

					// clear the vertex buffers of cubes
					for (int i = 0; i < barChart.size(); i++)
					{
						barChart[i].clearCube();
					}

					// clear bar chart vectors
					barChart.clear();
					barsX.clear();

					// create a new axes using new largest (0) i.e. this will create the axes seen on start up
					labels = newAxes.makeAxis(largest, 0);
					labelBoundary = largest; // set label boundary to largest to keep the axes names

					makeAxesNames(); // recreate the axes names to reposition them
				}

				/* if prevents checkboxes from appearing in certain graphtypes i.e.bar chart */
				if (graphType == 0)
				{
					ImGui::Dummy(ImVec2(0.0f, 2.f));

					// checkbox for 2D drawing
					if (ImGui::Checkbox("2D", &twoD))
					{
						threeD = !threeD; // change the 3D bool to its opposite
					}

					ImGui::SameLine(); // allows next item to be on the same line

					// checkbox for 3D drawing
					if (ImGui::Checkbox("3D", &threeD))
					{
						twoD = !twoD; // chage the 2D bool to its opposite
					}
				}

				ImGui::Dummy(ImVec2(0.0f, 5.f));

				/* ONLY DISPLAY WHEN GRAPH TYPE IS SCATTER PLOT */
				if (graphType == 0) {

					ImGui::SliderFloat("Point Size", &sizePoint, 5.0f, 20.f); // slider to edit the size of data points in a scatter graph
					ImGui::Dummy(ImVec2(0.0f, 5.f));
				}

				/* ONLY DISPLAY WHEN GRAPH TYPE IS LINE PLOT */
				if (graphType == 1) {
					ImGui::Checkbox("Line Loop", &drawmode); // changes drawmode bool to change the drawing mode used in render loop
					ImGui::Dummy(ImVec2(0.0f, 5.f));
				}

				/* Slider - Edits scaler which makes the whole scene larger or smaller */
				ImGui::SliderFloat("Graph Size", &scaler, 0.01f, 1.f);
				ImGui::Dummy(ImVec2(0.0f, 5.f));

				/* InputBox with Hint (Hint allows faded text upon startup) - Name for the X Axes (Flags allow the text to be all uppercase) */
				ImGui::InputTextWithHint("X Axes", "INSERT NAME HERE", Xlabel, IM_ARRAYSIZE(Xlabel), ImGuiInputTextFlags_CharsUppercase);

				ImGui::Dummy(ImVec2(0.0f, 5.f));

				/* InputBox with Hint (Hint allows faded text upon startup) - Name for the Y Axes (Flags allow the text to be all uppercase) */
				ImGui::InputTextWithHint("Y Axes", "INSERT NAME HERE", Ylabel, IM_ARRAYSIZE(Ylabel), ImGuiInputTextFlags_CharsUppercase);

				ImGui::Dummy(ImVec2(0.0f, 5.f));

				/* InputBox with Hint (Hint allows faded text upon startup) - Name for the Z Axes (Flags allow the text to be all uppercase) */
				ImGui::InputTextWithHint("Z Axes", "INSERT NAME HERE", Zlabel, IM_ARRAYSIZE(Zlabel), ImGuiInputTextFlags_CharsUppercase);

				/* CHECK IF ANY LABELS HAVE BEEN ENTER INTO THE TEXTBOXES */
				if (labelCheckX != Xlabel || labelCheckY != Ylabel || labelCheckZ != Zlabel) // since check vars were set in init function... any change will indicate they need to be updated
				{
					ImGui::Dummy(ImVec2(0.0f, 3.f));
					if (ImGui::Button("UPDATE LABELS")) // add a button to update the labels
					{
						labelCheckX = Xlabel; // update the label and make quads regarding that
						labelCheckY = Ylabel; // update the label and make quads regarding that
						labelCheckZ = Zlabel; // update the label and make quads regarding that
						makeAxesNames();
						// make new quads for this label
					}

				}

				ImGui::Dummy(ImVec2(0.0f, 5.f));

				/* RESET CAMERA POSITION TO ORIGIN UPON BUTTON PRESS */
				if (ImGui::Button("RESET POSITION"))
				{
					camX = 0;
					camY = 0;
					camZ = 0;
				}

				ImGui::SameLine(); // allows next item to be on the same line

				/* BUTTON TO CLEAR GRAPH DATA */
				if (ImGui::Button("Clear Graph"))
				{
					// clear data point vector
					clearGraphVector();

					// clear the notches on each axes
					newAxes.clearLabels();

					// clear the vertex buffers of the number quads
					for (int i = 0; i < numberLables.size(); i++)
					{
						numberLables[i].clearQuad();
					}

					// clear number quad vectors
					numberLables.clear();
					positiveLabels.clear();
					negativeLabels.clear();

					// clear the vertex buffers of cubes
					for (int i = 0; i < barChart.size(); i++)
					{
						barChart[i].clearCube();
					}

					// clear bar chart vectors
					barChart.clear();
					barsX.clear();

					// reset the camera position
					camX = 0;
					camY = 0;
					camZ = 0;

					// create a new axes using new largest (0) i.e. this will create the axes seen on start up
					labels = newAxes.makeAxis(largest, 0);
					labelBoundary = largest; // set label boundary to largest to keep the axes names

					makeAxesNames(); // recreate the axes names to reposition them

				}

				ImGui::Dummy(ImVec2(0.0f, 5.f));

				/* HEADING FOR COLOUR CHANGER */
				ImGui::Text("Data Point Colour");
				{
					ImGui::Dummy(ImVec2(0.0f, 5.f));

					if (ImGui::ColorEdit3(" ", color)) // ImGui Colour editor
					{
						if (!vertexPos.empty()) // check if position vector is empty (prevents accessing memory that doesn't exist)
						{
							vertexColours.clear(); // clear the colour vector (to add new colours to the vector)

							// insert the RBGA value for every point in the data point vector
							for (int i = 0; i < vertexPos.size(); i++)
							{
								vertexColours.insert(vertexColours.end(), { color[0] , color[1] , color[2] , color[3] }); // vector function insert allows to insert an array of floats (add these to the end of the vector)
							}
						}

						if (!barChart.empty()) // colour changing for the bar chart
						{
							// edit the colour buffer for each cube using function in cube class
							for (int i = 0; i < barChart.size(); i++)
							{
								barChart[i].editColour(color); // pass in the global colour array to be used in function
							}
						}

						/* Create Colour Buffer for data points */
						glGenBuffers(1, &plotColourBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
						glBufferData(GL_ARRAY_BUFFER, vertexColours.size() * sizeof(float), &(vertexColours[0]), GL_DYNAMIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}
				}
				ImGui::EndTabItem(); // signal end of menu tab
			}

			// new tab HELP (DISPLAYS USER CONTROLS AND GENERAL ADVICE)
			if (ImGui::BeginTabItem("HELP"))
			{
				ImGui::Dummy(ImVec2(0.0f, 5.f)); // padding

				// welcome text
				ImGui::TextWrapped("WELCOME TO MULTIDIMENSIONAL PLOTTER!");

				ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

				// sub heading
				ImGui::TextWrapped("A PLOTTING PROGRAM CREATE IN OPENGL (C++) BY MAX FYALL");

				ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

				ImGui::Unindent(10); // unindent the upcoming bullet points

				// Instruction Node
				if (ImGui::TreeNode("HOW DOES IT WORK?"))
				{
					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// instructions for moving around
					if (ImGui::TreeNode("MOVING AROUND"))
					{
						ImGui::Unindent(10); // unindent the upcoming bullet points

						// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("BY HOLDING DOWN MOUSE 3 (ALTERNATIVLY MOUSE 2) AND MOVING THE MOUSE, YOU CAN ROTATE THE GRAPH TO GET A DIFFERENT VIEW");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("USE THE SCROLL WHEEL TO ZOOM IN AND OUT TO GET A BETTER VIEW. ALTERNATIVELY, YOU CAN CHANGE THE SIZE OF THE GRAPH USING THE 'GRAPH SIZE' SLIDER");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("USING THE ARROW KEYS WILL ALLOW YOU TO MOVE TO GRAPH IN THE DESIRED DIRECTION. PRESSING THE 'RESET POSITION' BUTTON WILL RETURN THE GRAPH TO THE CENTRE");

						ImGui::TreePop(); // declare end of tree node
					}
					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					if (ImGui::TreeNode("PLOTTING"))
					{
						ImGui::Unindent(10); // unindent the upcoming bullet points

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("TO START PLOTTING, VISIT THE HOME TAB AND START BY SELECTING THE GRAPH YOU WANT TO PLOT. YOU CAN ALSO CHOOSE A 2D OR 3D TYPE OF GRAPH VIA THE CHECKBOXES");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("CLICKING THE 'OPEN FILE' BUTTON WILL PROMPT A FILE DIALOG BOX, FOR WHICH YOU CAN SELECT A TEXT FILE TO PLOT WITH. UPON OPENING, YOUR GRAPH WILL BE PLOTTED");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("PRESSING THE 'CLEAR GRAPH' BUTTON WILL CLEAR PLOTTED GRAPHS (RETURNING TO THE DEFAULT STATE BUT KEEPING ANY AXES LABELS)");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("IT MUST BE NOTED THAT ATTEMPTING TO CHANGE GRAPH TYPE WHEN A GRAPH IS ALREADY PLOTTED WILL RESULT IN THE CURRENT GRAPH BEING CLEARED");

						ImGui::TreePop(); // declare end of tree node
					}
					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					if (ImGui::TreeNode("DATA POINTS"))
					{
						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("WHEN PLOTTING USING SCATTER PLOT, YOU CAN CHANGE THE SIZE OF THE DATA POINTS TO SUIT YOUR LIKING");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("WHEN PLOTTING USING LINE GRAPH, YOU CAN TOGGLE 'LINE LOOP'. THIS LINKS ALL THE LINES TOGETHER");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("IT ALSO POSSIBLE TO USE THE COLOUR PICKER TO CHANGE THE COLOUR OF THE DATA POINTS TO YOU LIKING (THIS IS POSSIBLE ON ALL GRAPH TYPES)");

						ImGui::TreePop(); // declare end of tree node
					}
					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					if (ImGui::TreeNode("AXES LABELS"))
					{
						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("TO AN AXES NAME, TYPE THE NAME INTO THE RELEVENT BOX.");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("DOING THIS WILL CAUSE THE 'UPDATE' BUTTON TO APPEAR. CLICKING THIS WILL ADD YOUR NAME TO THE RELEVENT AXES");

						ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

						// Use bullet points (Wrapped text allows for text to adjust to window size)
						ImGui::Bullet(); ImGui::TextWrapped("YOU CAN CHANGE THE NAMES BY SIMPLY REPEATING THIS PROCESS");

						ImGui::TreePop(); // declare end of tree node
					}

					ImGui::TreePop(); // declare end of tree node

				}

				ImGui::Dummy(ImVec2(0.0f, 2.f)); // padding

				// User controls node
				if (ImGui::TreeNode("USER CONTROLS")) 
				{

					ImGui::Unindent(10); // unindent the upcoming bullet points

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("ESCAPE (ESC)\n(TERMINATE PROGRAM)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("MOUSE 3 (ALT - MOUSE 2)\n(Press and hold to unlock graph rotation)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("SCROLL UP\n(Zoom in)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("SCROLL DOWN\n(Zoom out)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("UP ARROW\n(Move graph up)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("DOWN ARROW\n(Move graph down)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("LEFT ARROW\n(Move graph left)");

					ImGui::Dummy(ImVec2(0.0f, 1.f)); // padding

					// Use bullet points for controls using \n to go to a new line (Wrapped text allows for text to adjust to window size)
					ImGui::Bullet(); ImGui::TextWrapped("RIGHT ARROW\n(Move graph right)");

					ImGui::TreePop(); // declare end of tree node
				}

				ImGui::EndTabItem(); // signal end of help tab
			}
		}
		ImGui::EndTabBar(); // signal end of Tab bar itself
	}
	ImGui::End(); // End - Signals the end of the window

	// render GUI onto the screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
// End of Display

/*
*  Resizing window callback
*/
static void reshape(GLFWwindow* window, int w, int h)
{
	// resize the window
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// aspect ratio change
	aspect_ratio = ((float)w / 640.f * 4.f) / ((float)h / 480.f * 3.f);
}

/*
*  Callback for Keyboard inputs
*/
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	// close application window with ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// print confirmation
		std::cout << "ESC PRESSED - TERMINATING" << std::endl;

		// set window close to true (breaks out of render loop)
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	/* CAMERA TRANSFORMATION KEYS */

	if (key == GLFW_KEY_UP) // move scene up if up-arrow is pressed
	{
		camY += 0.1f;
	}
	else if (key == GLFW_KEY_DOWN) // move scene down if down-arrow is pressed
	{
		camY -= 0.1f;
	}
	else if (key == GLFW_KEY_RIGHT) // move scene right if right-arrow is pressed
	{
		camX += 0.1f;
	}
	else if (key == GLFW_KEY_LEFT) // move scene left if left-arrow is pressed
	{
		camX -= 0.1f;
	}

}

/*
*  Mouse Callback Function (SEE REFERENCE [8])
*/
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	// if bool is true (changed by pressing Mouse 3/2)
	if (moveScene)
	{
		// set position variables
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		// if this is first time moving
		if (firstMouse)
		{
			// set position variables
			lastX = xpos;
			lastY = ypos;

			// change bool since first mouse is not needed
			firstMouse = false;
		}

		// set offsets
		double xoffset = xpos - lastX;
		double yoffset = lastY - ypos;

		// store position data
		lastX = xpos;
		lastY = ypos;

		// change pitch and yaw to rotate the camera
		pitch += xoffset * 0.1f;
		yaw += yoffset * 0.1f;

	}

}

/*
*	Mouse button callback (SEE REFERENCE [7])
*/
static void mouseButonCallback(GLFWwindow* window, int button, int action, int mods) 
{
	// if user presses mouse 3 or mouse 2
	if (button == GLFW_MOUSE_BUTTON_MIDDLE || button == GLFW_MOUSE_BUTTON_2)
	{
		// checks for action to allow for press and hold functionality
		if (action == GLFW_PRESS)
		{
			// set relevent booleans
			moveScene = true;
			firstMouse = true;
		}
		else if(action == GLFW_RELEASE) // when user releases button
		{
			// stop scene rotations
			moveScene = false;
		}

		// cursor settings
		if (moveScene)
		{
			// if the scene is rotating, make the cursor invisible
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);			
		}
		else
		{
			// make cursor visible when not rotating the scene
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	
}

/*
*  Scroll Callback function acquired from Learn OpenGL
*/
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// change camera position based on scroll
	camZ += (float)yoffset;

	// stop camera from going bellow 20 (HOW FAR OUT IT ZOOMS)
	if (camZ < -20.f)
		camZ = -20.f;

	// stop camera from going over 4 (HOW CLOSE IN IT ZOOMS)
	if (camZ > 4.f)
		camZ = 4.f;
}

/*
*  Read vertex positions from file - returns vector of floats
*  Also finds the largest value for creating 3-D axes to fit points provided
*/
std::vector<float> readData(std::string filePath)
{
	// create a variables to use
	std::vector<std::string> vertexPositions;
	bool invalid = false;
	int numberofBars = 0;

	// create filestream using filepath passed in
	std::ifstream filestream(filePath);

	// if unable to find file
	if (!filestream.is_open())
	{
		// Print error
		std::cout << "Could not read data file: " << filePath << ". File does not exist. " << std::endl;

		// create empty vector to pass
		std::vector<float> empty;
		empty.push_back(0); // insert 0 to prevent memory exceptions
		filestream.close(); // close the filestream
		return empty; // return the empty vector
	}

	// string used in file reading process
	std::string line;
	while (!filestream.eof()) // loop through the file
	{
		// get line from file
		getline(filestream, line);

		// add it to our vector of strings
		vertexPositions.push_back(line);
	}

	// close the filestream when finished
	filestream.close();

	std::vector<float> plotPos; // create vector of floats

	// loop through string vector (holds data) and add each data point into float vector
	for (int i = 0; i < vertexPositions.size(); i++)
	{
		// set variable to current string in vector
		std::string temp = vertexPositions[i];

		/* Splitting string using string stream(SEE REFERENCE [9]) */

		// string variable for splitting string
		std::string num;

		// create stringstream using temp string (temp string being a number of points in string format)
		std::stringstream ss(temp);

		// set bar variable to zero (used to calc number of bars in the x direction)
		numberofBars = 0;

		// loop through the string stream to add each data point into the float vector
		while (ss >> num)
		{
			
			// loop through string num to find if it contains any letters
			for (int i = 0; i < num.length(); i++)
			{
				// check if char is letter, or is not a special character that is not '-' or '.'
				if (isalpha(num[i]) || (!isalpha(num[i]) && !isdigit(num[i]) && num[i] != '-' && num[i] != '.'))
				{
					// if string does contain a invalid character (i.e. a letter or symbol rather than a number)
					invalid = true;
					break;
				}
			}
			
			// if the string is invalid
			if (invalid)
			{
				// do not add the data point
				//std::cout << "INVALID DATA POINT - REMOVING: " << num << std::endl;
				invalid = false;
			}
			else
			{
				// otherwise convert from string to float and add to float vector
				plotPos.push_back(std::stof(num));

				// calculate largest value
				if (std::stof(num) > largest)
				{
					largest = std::stof(num);
				}
				// if all values in file are negative we need to check what the "smallest" value is
				else if(-(std::stof(num)) > largest)
				{
					largest = -std::stof(num);
				}

				// increase bar counter - (Number of values in the X direction)
				numberofBars++;
			}

		}
		// if number of bars is 0 nothing was in that line of the file
		if (numberofBars != 0)
		{
			// insert bar value into barchart vector
			barsX.insert(barsX.end(), numberofBars);
		}

	}

	// clear the colour vector if it is not empty
	if (!vertexColours.empty())
	{
		vertexColours.clear();
	}

	// insert an RGBA value for each data point
	for (int i = 0; i < plotPos.size(); i++)
	{
		vertexColours.insert(vertexColours.end(), { color[0] , color[1] , color[2] , color[3] });
	}

	// check if float vector is empty
	if (plotPos.empty())
	{
		// set labelboundary for axes names
		labelBoundary = largest;

		// create an empty vector to return
		std::vector<float> check;
		check.push_back(0);
		return check;
	}
	else
	{
		// set labelboundary for axes names
		labelBoundary = largest;

		// return float vector
		return plotPos;
	}
}

/*
* Clears the plot vector and creates new buffers to clear data on screen
*/
void clearGraphVector() 
{
	// set labelboundary for axes names
	labelBoundary = largest;

	// clear data point vector
	std::cout << "CLEAR THE GRAPH" << std::endl;
	vertexPos.clear();
	vertexPos.push_back(0);

	// set size and largest variables
	size = 0;
	largest = 0;

	/* CREATE DATA POINT BUFFER FROM EMPTY VECTOR */
	glGenBuffers(1, &plotBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &(vertexPos[0]), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// clear vector again to fully clear it since vector has one value (0)
	vertexPos.clear();
}

/*
* Splits Label Vector into 2 different Vectors
*/
void splitLabelVectors() 
{
	// loop through the label vector inserting positive numbers into one vector and negative vectors into another
	for (int i = 0; i < labels.size(); i++)
	{
		// check if vector is empty to prevent memory exceptions
		if (!labels.empty())
		{
			// check for negative number
			if (std::stof(labels[i]) < 0)
			{
				// insert into negative vector (convert to int allows numbers to be to one decimal place i.e. 2 instead of 2.000000)
				negativeLabels.push_back(std::to_string(std::stoi(labels[i])));
			}
			else
			{
				// insert into positive vector (convert to int allows numbers to be to one decimal place i.e. 2 instead of 2.000000)
				positiveLabels.push_back(std::to_string(std::stoi(labels[i])));
			}
		}
	}
	// reverse the negtive vector due to drawing order
	std::reverse(negativeLabels.begin(), negativeLabels.end());
}


/*
* Creates Quads to fit number and negative symbol. Quads are added to quad vector to be drawn in render loop.
* Old Method was problem since this would be called in render loop, leaking memory as each quad was made.
*/
void createNumberLabels() 
{
	// initialise variables used in algorithm
	float yBump = 0;
	int count = 0;
	std::string::const_iterator test;
	std::string number;

	// set temp to indent variable (bump having being set previously)
	float temp = bump;

	// loop for each axes - creating a new quads for each positive and negative label
	for (int j = 0; j < 3; j++)
	{
		// create a quad for each positive number for current axes
		for (int i = 0; i < positiveLabels.size(); i++)
		{
			// create variable to track size of number i.e. does it require more than 1 quad to draw (10)
			float cycle = 0;

			// check if vector is empty - prevents memory exceptions
			if (!labels.empty())
			{
				// get out first number
				number = positiveLabels[i];

				// set indentation for y axes labels
				yBump = 0;

				// loop through the string using string iterator
				for (test = number.begin(); test != number.end(); test++)
				{
					// check if we are on the Y axes
					if (j == 1)
					{
						// if cycle is higher than one the number is more than one digit
						if (cycle >= 1)
						{
							// increase indent by 0.1
							yBump = yBump + 0.1f;
						}
					}
					else // for x and z axes
					{
						// if cycle is higher than one the number is more than one digit
						if (cycle >= 1 && j != 1) 
						{
							// increase indent by 0.1
							bump = bump + 0.1f;
						}
					}

					// insert a quad object into quad vector
					numberLables.insert(numberLables.end(), newQuad);

					// using tracker variable make a quad using variables to decide which quad to make
					// - bump indicates where to draw the quad
					// - 1 indicates what size of quad to draw (i.e. normal or small)
					// - j is which axes to draw for
					// - yBump is used for numbers on the y axes longer than one digit
					numberLables[count].makeQuad(bump, 1, j, yBump);

					// add one the counter to increase quad count
					count++;

					// increase cycle by one since we have completed a cycle
					cycle = cycle + 1;

				}

				// reset indent variable back to original status i.e. status before the loop
				if (cycle > 1 && j != 1)
				{
					// decrease bump by the number of cycles minus one (since an extra cycle is added on when it didn't really happen)
					bump = bump - (0.1f * (cycle - 1));
				}

				// reset y axes indent variable
				yBump = 0;

				// move along current axes to make quads in correct location i.e. move from 1 to 2 or 2 to 4 in world space
				// addby can either be 1 or 2 (this is due to the gap between axes notches having variable size depending on the largest value)
				bump = bump + addby;

			}

		} // end of positive label loop
	
		// resting indent
		// Due to the indent vaariable being at the end notch we can use this to start drawing the negative numbers if we make the indent var negative... hence the reason for reversing the order of the negative vector
		bump = bump - addby;

		// loop through negative labels
		for (int i = 0; i < negativeLabels.size(); i++)
		{
			// create new cycle variable
			float cycle2 = 0;

			// check if vector is empty - prevents memory exceptions
			if (!labels.empty())
			{
				// get out first number
				number = negativeLabels[i];

				// set indentation for y axes labels
				yBump = 0;

				// loop through the number string using string iterator
				for (test = number.begin(); test != number.end(); test++)
				{
					// check if we are on the Y axes
					if (j == 1)
					{
						// if cycle is higher than one the number is more than one digit
						if (cycle2 >= 1)
						{
							// increase indent by 0.1
							yBump = yBump + 0.1f;
						}
					}
					else // for x and z axes
					{
						// if cycle is higher than one the number is more than one digit
						if (cycle2 >= 1 && j != 1)
						{
							// increase indent by 0.1
							bump = bump - 0.1f;
						}
					}

					// insert a quad object into our quad vector
					numberLables.insert(numberLables.end(), newQuad);

					// check if we are at the start of the number i.e. since it is negative this will be a '-'
					if (cycle2 == 0)
					{
						// since the '-' will become distored on a normal size quad we need to make a smaller one to fit it
						// so we pass in 0 instead on 1 to make it smaller
						// Note we pass negative bump into the function since we are on the negative side of the axes
						numberLables[count].makeQuad(-(bump), 0, j, yBump);
					}
					else
					{
						// otherwise we have a regular number and can make a regular quad for the number
						numberLables[count].makeQuad(-(bump), 1, j, yBump);
					}

					// add one the counter to increase quad count
					count++;

					// increase cycle by one since we have completed a cycle
					cycle2 = cycle2 + 1;

				}

				// reset indent variable back to original status i.e. status before the loop
				if (cycle2 > 1 && j!= 1)
				{
					// decrease bump by the number of cycles minus one (since an extra cycle is added on when it didn't really happen)
					// NOTE - adding back to the variable since we took it away for the negative numbers
					bump = bump + (0.1f * (cycle2 - 1));
				}

				// reset y axes indent variable
				yBump = 0;


				// move along current axes to make quads in correct location i.e. move from -10 to -9 or -9 to -8 in world space
				// addby can either be 1 or 2 (this is due to the gap between axes notches having variable size depending on the largest value)
				bump = bump - addby;

			}
		} // end of negative label loop

		// reset indent variable to value before looping for each axes
		bump = temp;

	} // end of axes loop

} // end of function

/*
* Create Quads for the Axes names
*/
void makeAxesNames() 
{
	// check if any of the vectors are not empty
	if (!XAxesLabel.empty() || !YAxesLabel.empty() || !ZAxesLabel.empty())
	{
		// clear the buffers for each quad inside the vectors
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

		// clear the vectors
		XAxesLabel.clear();
		YAxesLabel.clear();
		ZAxesLabel.clear();
	}

	// variables used for algorithm
	std::string::const_iterator t;
	int loop = 0;
	float indent = 0;

	// set indentation variable to the axes edge (Label boundary + 1.1 is just on the tip of the axes edge)
	indent = labelBoundary + 1.1f;
	
	// using iterator, loop through the label string
	for (t = labelCheckX.begin(); t != labelCheckX.end() ; t++)
	{
		// Check if current letter is 'I'
		// 'I' needs to have quad size adjusted due to it being too small
		if (labelCheckX[loop] == 'I' )
		{
			// insert quad object into quad vector
			XAxesLabel.insert(XAxesLabel.end(), newQuad);

			// create newly added quad using indent and passing 2 for 'I' sized quad
			// 0 for x axes and we dont require a y indent so again 0
			XAxesLabel[loop].makeQuad(indent, 2, 0, 0);

			// increase loop by one to move to next quad
			loop++;

			// increase indentation to move next quad for the next character
			indent = indent + 0.1f;
		}
		else // for every other character besides 'I'
		{
			// insert quad object into quad vector
			XAxesLabel.insert(XAxesLabel.end(), newQuad);

			// create newly added quad using indent and passing 1 for normal sized quad
			// 0 for x axes and we dont require a y indent so again 0
			XAxesLabel[loop].makeQuad(indent, 1, 0, 0);

			// increase loop by one to move to next quad
			loop++;

			// increase indentation to move next quad for the next character
			indent = indent + 0.1f;
		}
	}

	// reset loop and initialise y axes indentation
	loop = 0;
	float yIndent = 0.1f;

	// reset indentation variable
	indent = labelBoundary + 1.1f;

	// using iterator, loop through the label string
	for (t = labelCheckY.begin(); t != labelCheckY.end(); t++)
	{
		// Check if current letter is 'I'
		// 'I' needs to have quad size adjusted due to it being too small
		if (labelCheckY[loop] == 'I')
		{
			// insert quad object into quad vector
			YAxesLabel.insert(YAxesLabel.end(), newQuad);

			// create newly added quad using indent and passing 2 for 'I' sized quad
			// 1 for y axes and we do require a y indent so pass the y indent variable as well.
			YAxesLabel[loop].makeQuad(indent, 2, 1, yIndent);

			// increase loop by one to move to next quad
			loop++;

			// increase y indentation to move next quad for the next character
			yIndent = yIndent + 0.1f;
		}
		else  // for every other character
		{
			// insert quad object into quad vector
			YAxesLabel.insert(YAxesLabel.end(), newQuad);

			// create newly added quad using indent and passing 1 for normal sized quad
			// 1 for y axes and we do require a y indent so pass the y indent variable as well.
			YAxesLabel[loop].makeQuad(indent, 1, 1, yIndent);

			// increase loop by one to move to next quad
			loop++;

			// increase y indentation to move next quad for the next character
			yIndent = yIndent + 0.1f;
		}
	}

	// reset loop and initialise new variables
	// due to the nature of the bar chart we need to establish making the label on both sides of the z axes
	loop = 0;
	float labelSize = labelCheckZ.size();

	// if our graph type is bar chart
	if (graphType == 2)
	{
		// calculate where the axes name should go on the left side of the z axes
		// Since we texture the name from left to right, we calculate how far the name will go by the size of the string
		indent = (-labelBoundary) - ((labelSize)/10) - 1.1;
	}
	else // for every other graph type (scatter and line)
	{
		// set indendation for the right side of the z axes
		indent = labelBoundary + 1.1f;
	}
	
	// using iterator, loop through the label string
	for (t = labelCheckZ.begin(); t != labelCheckZ.end(); t++)
	{
		// Check if current letter is 'I'
		// 'I' needs to have quad size adjusted due to it being too small
		if (labelCheckZ[loop] == 'I')
		{
			// insert quad object into quad vector
			ZAxesLabel.insert(ZAxesLabel.end(), newQuad);

			// create newly added quad using indent and passing 2 for 'I' sized quad
			// 2 for z axes and we dont require a y indent so again 0
			ZAxesLabel[loop].makeQuad(indent, 2, 2, 0);

			// increase loop by one to move to next quad
			loop++;

			// increase indentation to move next quad for the next character
			indent = indent + 0.1f;
		}
		else // for every other character
		{
			// insert quad object into quad vector
			ZAxesLabel.insert(ZAxesLabel.end(), newQuad);
		
			// create newly added quad using indent and passing 1 for normal sized quad
			// 2 for z axes and we dont require a y indent so again 0
			ZAxesLabel[loop].makeQuad(indent, 1, 2, 0);
			
			// increase loop by one to move to next quad
			loop++;

			// increase indentation to move next quad for the next character
			indent = indent + 0.1f;
		}
	}
}

/*
* Add and create cubes for a bar chart
*/
void createBars() 
{
	// loop through bars vector for size to get number in the z
	// loop again to get number in the x

	// initialise variables
	int q = 0;
	int moveZ = 0;
	int moveX = 0;

	// loop for bar vector size which represents the number of bars in the Z direction (NUMBER OF ROWS)
	for (int i = 0; i < barsX.size(); i++)
	{
		// initialise x indent variable, since each row starts at 0
		moveX = 0;

		// the integer value in each value of the vector represents the number of bars in the X direction (NUMBER OF COLUMNS)
		if (barsX[i] != 0)
		{
			// loop for the number of columns
			for (int j = 0; j < barsX[i]; j++)
			{
				// insert a cube into the bar chart cube vector
				barChart.insert(barChart.end(), testCube);

				// make the cube inserted passing in correct parameters
				// vertexPos vector - data point read in from file to dictate the height of the cube
				// moveX float - how much to move the cube in the x direction (i.e. moving the cube to the next notch on x axes)
				// moveZ float - how much to move the cube in the z direction (i.e. are we on the first row or the second row?)
				// addby float - specified number, how much to move the cubes by (This is dependant on the axes configuration)
				// colour float array - the colour to pass to the colour buffer
				// NOTE - x and z positions are multiplied by 4 to give the desired effects in the scene
				barChart[q].makeCube((vertexPos[q]), ((moveX + 1)) * 4, (-(moveZ + 1) * 4), color);

				// increment by one to move to the next cube
				q++;

				// increment x indentation by addby variable to move along the x axes
				moveX = moveX + 1;

			}
			// once all bars have been drawn in the x direction, increment the z direction by addby variable to move down the z axes
			moveZ = moveZ + 1;
		}

	}

}

/*
*  Main Method, program control point
*/
int main(int argc, char* argv[])
{
	// create instance of GLW wrapper class, specifying dimensions of the GLFW window and the Name
	GLWrapper* glw = new GLWrapper(1024, 768, "Multidimensional Plotter");

	// if functions fail to load terminate the program
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "ogl_LoadFunctions() failed, Terminating\n");
		return 0;
	}

	// setting glfw callbacks and functions to be used in the wrapper class
	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setMouseCallback(mouseCallback);
	glw->setMouseButtonCallback(mouseButonCallback);
	glw->setScrollCallback(scrollCallback);
	glw->setReshapeCallback(reshape);
	glw->DisplayVersion();

	// call init passing in wrapper window
	init(glw);

	// run the event loop
	glw->eventLoop();

	// delete wrapper window when done
	delete(glw);
	return 0;
}
