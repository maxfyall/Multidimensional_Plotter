/* cube.h
 Example class to create a generic cube object
 Iain Martin October 2018

EXTENDED BY Max Fyall - 180011724
Multidimensional Plotter - 2022
*/

#include "cube.h"

/* I don't like using namespaces in header files but have less issues with them in
seperate cpp files */
using namespace std;

/* Define the vertex attributes for vertex positions and normals.
Make these match your application and vertex shader
You might also want to add colours and texture coordinates */
Cube::Cube()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	numvertices = 12;
}


Cube::~Cube()
{
}


/* Make a cube from hard-coded vertex positions */
void Cube::makeCube(float height, int moveX, int moveZ, static float colour[4])
{
	// define height if height is 0
	if (height == 0)
	{
		height = height + 0.01;
	}

	/* Define vertices for a cube in 12 triangles */
	GLfloat vertexPositions[] =
	{
		
		// each vertex is defined using variables passed in from calling the function
		// these are used to position the cube exactly where we want it to be
		// modifying the correct planes allows use to make the height of the cube bigger or smaller
		// This gives us bar chart like functionality over the cubes
		-0.25f+moveX, (height), -0.25f+moveZ,
		-0.25f+moveX, -0.25f, -0.25f+moveZ,
		0.25f+moveX, -0.25f, -0.25f+moveZ,

		0.25f+moveX, -0.25f, -0.25f+moveZ,
		0.25f+moveX, (height), -0.25f+moveZ,
		-0.25f+moveX, (height), -0.25f+moveZ,

		0.25f+moveX, -0.25f, -0.25f+moveZ,
		0.25f+moveX, -0.25f, 0.25f+moveZ,
		0.25f+moveX, (height), -0.25f+moveZ,

		0.25f+moveX, -0.25f, 0.25f+moveZ,
		0.25f+moveX, (height), 0.25f+moveZ,
		0.25f+moveX, (height), -0.25f+moveZ,

		0.25f+moveX, -0.25f, 0.25f+moveZ,
		-0.25f+moveX, -0.25f, 0.25f+moveZ,
		0.25f+moveX, (height), 0.25f+moveZ,

		-0.25f+moveX, -0.25f, 0.25f+moveZ,
		-0.25f+moveX, (height), 0.25f+moveZ,
		0.25f+moveX, (height), 0.25f+moveZ,

		-0.25f+moveX, -0.25f, 0.25f+moveZ,
		-0.25f+moveX, -0.25f, -0.25f+moveZ,
		-0.25f+moveX, (height), 0.25f+moveZ,

		-0.25f+moveX, -0.25f, -0.25f+moveZ,
		-0.25f+moveX, (height), -0.25f+moveZ,
		-0.25f+moveX, (height), 0.25f+moveZ,

		-0.25f+moveX, -0.25f, 0.25f+moveZ,
		0.25f+moveX, -0.25f, 0.25f+moveZ,
		0.25f+moveX, -0.25f, -0.25f+moveZ,

		0.25f+moveX, -0.25f, -0.25f+moveZ,
		-0.25f+moveX, -0.25f, -0.25f+moveZ,
		-0.25f+moveX, -0.25f, 0.25f+moveZ,

		-0.25f+moveX, (height), -0.25f+moveZ,
		0.25f+moveX, (height), -0.25f+moveZ,
		0.25f+moveX, (height), 0.25f+moveZ,

		0.25f+moveX, (height), 0.25f+moveZ,
		-0.25f+moveX, (height), 0.25f+moveZ,
		-0.25f+moveX, (height), -0.25f+moveZ,
	};

	/* Manually specified colours for our cube */
	float vertexColours[] = {

		// each vertex in the cube will be the same colour
		// defined by the float array past in when calling the function
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],

		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],

		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],

		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],

		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],

		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
		colour[0], colour[1], colour[2], colour[3],
	};

	/* Create the vertex buffer for the cube */
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the colours buffer for the cube */
	glGenBuffers(1, &colourObject);
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

/*
* Create new colour buffer with float array
*/
void Cube::editColour(static float colour[4]) 
{
	float vertexColours[] = {

	// define vertex colours using colour array
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],

	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],

	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],

	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],

	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],

	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	colour[0], colour[1], colour[2], colour[3],
	};

	/* Create new colour buffer for the cube */
	glGenBuffers(1, &colourObject);
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/* Draw the cube by binding the VBOs and drawing triangles */
void Cube::drawCube(int drawmode)
{
	/* Bind cube vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind cube colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glPointSize(3.f);

	// Switch between filled and wireframe modes
	if (drawmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw points
	if (drawmode == 2)
	{
		glDrawArrays(GL_POINTS, 0, numvertices * 3);
	}
	else // Draw the cube in triangles
	{
		glDrawArrays(GL_TRIANGLES, 0, numvertices * 3);
	}
}

/*
* Clear the vertex buffers of the cube
*/
void Cube::clearCube() 
{
	// create empty vector to copy into the vertex buffer
	std::vector<GLfloat> vertexPositions = { 0 };

	/* Create the empty vertex buffer for the cube */
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(GLfloat), &(vertexPositions[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create empty vector to copy into the colour buffer
	std::vector<GLfloat> vertexColours = { 0 };

	/* Create the empty colour buffer for the cube */
	glGenBuffers(1, &colourObject);
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glBufferData(GL_ARRAY_BUFFER, vertexColours.size() * sizeof(GLfloat), &(vertexColours[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}