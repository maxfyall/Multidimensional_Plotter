/*
	Max Fyall - 180011724
	Multidimensional Plotter
*/

/*	REFERENCES USED:

	[1] Freetype Code Reference - https://learnopengl.com/In-Practice/Text-Rendering
	[2] Freetype Library - https://freetype.org/
	[3] ImGui Documentatation - https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html
	[4] Open File using Window API Tutorial - https://www.youtube.com/watch?v=-iMGhSlvIR0
	[5] Microsoft Documentation - https://docs.microsoft.com/en-us/answers/questions/483237/a-value-of-type-34const-char-34-cannot-be-assigned.html
	[6] GLFW Mouse Button Callback - https://www.glfw.org/docs/3.3/input_guide.html

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

/* Include standard C++ libraries for help in functions */
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

/* Include Header Files of required classes */
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
ThreeDAxes newAxes; // Axes class instance, draws the X, Y, Z axes
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

	//glEnable(GL_CULL_FACE);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* FREETYPE LIBRARY INITIALISING - AQUIRED FROM LEARNOPENGL - https://learnopengl.com/In-Practice/Text-Rendering */

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
	labels = newAxes.makeAxes(largest); // make axes returns vector of floats containing the labels required for x y z axes

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

	// create a stack of 4D matrices (used in transformations)
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
	if (largest < 9)
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

	// send the uniform matricies to the vertex shader (used in model-view-projection calculation)
	glUniformMatrix4fv(viewID1, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID1, 1, GL_FALSE, &projection3D[0][0]);

	// apply transformations to the whole scene
	model.top() = glm::scale(model.top(), glm::vec3(scaler, scaler, scaler)); // scale the scene using the variable editted by ImGui
	model.top() = glm::translate(model.top(), glm::vec3(camX, camY, 0)); // apply a translate using variable update by the user (using the arrow keys)

	/* SCENE ROTATIONS */
	model.top() = glm::rotate(model.top(), glm::radians(yaw), glm::vec3(1, 0.0f, 0.0f)); // mouse rotation X
	model.top() = glm::rotate(model.top(), glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.0f)); // mouse rotation Y

	/* DRAWS THE AXES */
	model.push(model.top()); // push an indentity matrix onto the stack apply transformations to drawn items within
	{	
		// send model uniform matrix to the vertex shader with applied transformations
		glUniformMatrix4fv(modelID1, 1, GL_FALSE, &model.top()[0][0]);

		// draw the 3D axes using the axes class instance
		newAxes.drawAxes();

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

		// check for 2D or 3D drawing
		if (twoD)
		{
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // specify 2 components
		}
		else if (threeD)
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // specify 3 components
		}

		/* Create Colour Buffer for data points */ 
		glGenBuffers(1, &plotColourBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, plotColourBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexColours.size() * sizeof(float), &(vertexColours[0]), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

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


	/* IMGUI CODE - CREATION OF IMGUI FEATURES AND FUNCTIONALITY  (SEE REFERENCE [3]) */

	// Begin - push a window
	ImGui::Begin("MULTIDIMENSIONAL PLOTTER");
	{
		// welcome text
		ImGui::Text("Welcome to Multidimensional Plotter");

		ImGui::Dummy(ImVec2(0.0f, 10.f)); // padding

		// Open a file (using file dialog box) to read in using windows.h api (SEE REFERENCE [4], [5])
		if (ImGui::Button("Open File")) // if button is triggered perform tasks
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

			//std::cout << path << std::endl;

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

				// create our axes and set its return value (vector of strings) to labels vector.
				labels = newAxes.makeAxes(largest);

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
		}

		/* if prevents checkboxes from appearing in certain graphtypes i.e.bar chart */
		if (graphType != 2)
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

			// create a new axes using new largest (0) i.e. this will create the axes seen on start up
			labels = newAxes.makeAxes(largest);
			labelBoundary = largest; // set label boundary to largest to keep the axes names

			makeAxesNames(); // recreate the axes names to reposition them

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

			if (!barChart.empty())
			{
				for (int i = 0; i < barChart.size(); i++) 
				{
					barChart[i].editColour(color);
				}
			}
		}
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
// End of Display

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
std::vector<float> readData(std::string filePath)
{
	std::vector<std::string> vertexPositions;
	bool invalid = false;
	int numberofBarsZ = 0;

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
				std::cout << "INVALID DATA POINT - REMOVING" << std::endl;
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

	//size = vertexPos.size();
	size = 0;

	largest = 0;

	glGenBuffers(1, &plotBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, plotBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(float), &(vertexPos[0]), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vertexPos.clear();
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
	int limit = 3;
	std::string::const_iterator test;


	std::string jim;
	float temp = bump;

	for (int j = 0; j < limit; j++)
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
	float indent = 0;


	indent = labelBoundary + 1.1f;
	

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
	float nextChar = 0;
	float labelSize = labelCheckZ.size();

	if (graphType == 2)
	{
		indent = (-labelBoundary) - ((labelSize)/10) - 1;
		nextChar = 0.1f;
	}
	else 
	{
		indent = labelBoundary + 1.1f;
		nextChar = 0.1f;
	}
	
	std::cout << indent << std::endl;

	for (t = labelCheckZ.begin(); t != labelCheckZ.end(); t++)
	{
		labelCheckZ[loop] = std::toupper(labelCheckZ[loop]);

		if (labelCheckZ[loop] == 'I')
		{
			ZAxesLabel.insert(ZAxesLabel.end(), newQuad);
			ZAxesLabel[loop].makeQuad(indent, 2, 2, 0);
			loop++;
			indent = indent + nextChar;
		}
		else
		{
			labelCheckZ[loop] = std::toupper(labelCheckZ[loop]);
			ZAxesLabel.insert(ZAxesLabel.end(), newQuad);
		
			ZAxesLabel[loop].makeQuad(indent, 1, 2, 0);
			
			loop++;
			indent = indent + nextChar;

		}
	}

	// loop through each label string (Starting with X)
	// insert a new quad object into the quad vector the quads belong to i.e. x axes name
	// create a quad for each character in that string

}

void createBars() 
{
	// loop through bars vector for size to get number in the z
	// loop again to get number in the x

	int q = 0;
	int moveZ = 0;
	int moveX = 0;

	for (int i = 0; i < barsX.size(); i++)
	{
		moveX = 0;

		if (barsX[i] == 0)
		{
			std::cout << "breaking" << std::endl;
		}
		else 
		{
			for (int j = 0; j < barsX[i]; j++)
			{
				barChart.insert(barChart.end(), testCube);
				barChart[q].makeCube((vertexPos[q]), ((moveX + addby)) * 4, (-(moveZ + addby) * 4), color);
				//std::cout << "BAR WITH: " << vertexPos[q] << std::endl;
				q++;
				moveX = moveX + addby;

			}
			moveZ = moveZ + addby;
		}

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
	glw->setMouseButtonCallback(mouseButonCallback);
	glw->setScrollCallback(scrollCallback);
	glw->setReshapeCallback(reshape);
	glw->DisplayVersion();

	init(glw);

	glw->eventLoop();


	delete(glw);
	return 0;
}
