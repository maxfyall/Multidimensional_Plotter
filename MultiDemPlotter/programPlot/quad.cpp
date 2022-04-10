/*
  Quad Class - Create Quad for texturing characters onto

  Max Fyall - 180011724
  Multidimensional Plotter
*/

#include "quad.h"

/* Establish public global variables */
Quad::Quad() 
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	attribute_v_textures = 2;
}

/* Deconstructor not used */
Quad::~Quad() 
{

}

/*
* Create vertex buffers from vertex positions made with passed in variables
*/
void Quad::makeQuad(float bump, int small, int dir, float yBump)
{
	// clear the vertex position vector (it may contain one value to prevent memory access exceptions)
	quad.clear();

	// variable dir allows us to make a quad for either x, y or z axes
	// in this case if dir is 0, we make the quad for the x axes
	if (dir == 0)
	{
		// variable small allows us to make a quad to fit certain characters, mainly the letter 'I' and symbol '-'
		// in this case, we make a quad for '-' for negative numbers
		if (small == 1)
		{
			// create vertex positions by inserting quad coordinates (modified by indent variable on the x axes) into vertex position vector
			quad.insert(quad.end(), { (0.0f + bump), 0.1f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.1f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.3f, 0.f });
			quad.insert(quad.end(), { (0.0f + bump), 0.3f, 0.f });
		}
		// in this case, we make a quad for the letter 'I'
		else if (small == 2)
		{
			// create vertex positions by inserting quad coordinates (modified by indent variable on the x axes) into vertex position vector
			quad.insert(quad.end(), { (0.07f + bump-0.03f), 0.1f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump-0.03f), 0.1f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump-0.03f), 0.3f, 0.f });
			quad.insert(quad.end(), { (0.07f + bump-0.03f), 0.3f, 0.f });
		}
		// this case we use for every other character
		else
		{
			// create vertex positions by inserting quad coordinates (modified by indent variable on the x axes) into vertex position vector
			quad.insert(quad.end(), { (0.0f + bump), 0.2f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.2f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.21f, 0.f });
			quad.insert(quad.end(), { (0.0f + bump), 0.21f, 0.f });
		}
	}
	// in this case if dir is 1, we make the quad for the y axes
	else if(dir == 1)
	{
		// variable small allows us to make a quad to fit certain characters, mainly the letter 'I' and symbol '-'
		// in this case, we make a quad for '-' for negative numbers
		if (small == 1)
		{
			// create vertex positions by inserting quad coordinates (modified by indent variables on the y and x axes, to make multiple characters work on the same line) into vertex position vector
			quad.insert(quad.end(), { (0.09f+yBump), (0.2f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.2f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.4f+bump), 0.f });
			quad.insert(quad.end(), { (0.09f+yBump), (0.4f+bump), 0.f });
		}
		// in this case, we make a quad for the letter 'I'
		else if (small == 2)
		{
			// create vertex positions by inserting quad coordinates (modified by indent variables on the y and x axes, to make multiple characters work on the same line) into vertex position vector
			quad.insert(quad.end(), { (0.16f + yBump-0.03f), (0.2f + bump), 0.f });
			quad.insert(quad.end(), { (0.18f + yBump-0.03f), (0.2f + bump), 0.f });
			quad.insert(quad.end(), { (0.18f + yBump-0.03f), (0.4f + bump), 0.f });
			quad.insert(quad.end(), { (0.16f + yBump-0.03f), (0.4f + bump), 0.f });
		}
		// this case we use for every other character
		else
		{
			// create vertex positions by inserting quad coordinates (modified by indent variables on the y and x axes, to make multiple characters work on the same line) into vertex position vector
			quad.insert(quad.end(), { (0.09f+yBump), (0.3f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.3f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.31f+bump), 0.f });
			quad.insert(quad.end(), { (0.09f+yBump), (0.31f+bump), 0.f });
		}
	}
	// in this case if dir is 2, we make the quad for the z axes
	else if (dir == 2) 
	{
		// variable small allows us to make a quad to fit certain characters, mainly the letter 'I' and symbol '-'
		// in this case, we make a quad for '-' for negative numbers
		if (small == 1)
		{
			// create vertex positions by inserting quad coordinates (modified by indent variable on the z axes) into vertex position vector
			quad.insert(quad.end(), { 0.0f, 0.1f, (0.f + bump) });
			quad.insert(quad.end(), { 0.0f, 0.1f, (0.09f + bump) });
			quad.insert(quad.end(), { 0.0f, 0.3f, (0.09f + bump) });
			quad.insert(quad.end(), { 0.0f, 0.3f, (0.f + bump) });
		}
		// in this case, we make a quad for the letter 'I'
		else if (small == 2)
		{
			// create vertex positions by inserting quad coordinates (modified by indent variable on the z axes) into vertex position vector
			quad.insert(quad.end(), { 0.0f, 0.1f, (0.07f + bump - 0.03f) });
			quad.insert(quad.end(), { 0.0f, 0.1f, (0.09f + bump - 0.03f) });
			quad.insert(quad.end(), { 0.0f, 0.3f, (0.09f + bump - 0.03f) });
			quad.insert(quad.end(), { 0.0f, 0.3f, (0.07f + bump - 0.03f) });
		}
		// this case we use for every other character
		else
		{
			// create vertex positions by inserting quad coordinates (modified by indent variable on the z axes) into vertex position vector
			quad.insert(quad.end(), { 0.0f, 0.2f, (0.f + bump) });
			quad.insert(quad.end(), { 0.f, 0.2f, (0.09f + bump) });
			quad.insert(quad.end(), { 0.f, 0.21f, (0.09f + bump) });
			quad.insert(quad.end(), { 0.0f, 0.21f, (0.f + bump) });
		}
	}

	// define hard code colour values for each vertex
	GLfloat quadColour[] =
	{
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0

	};

	// define texture coordinates
	GLfloat quadTextCoord[] =
	{
		0.0f, 1.0f, 0,
		1.0f, 1.0f, 0,
		1.0f, 0.0f, 0,
		0.0f, 0.0f, 0,
	};

	/* Create Vertex Buffer for Quad */
	glGenBuffers(1, &quadBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadBO);
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), &(quad[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Colour Buffer for Quad */
	glGenBuffers(1, &quadColourBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadColourBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColour), quadColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Texture Buffer for Quad */
	glGenBuffers(1, &quadTextBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTextBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextCoord), quadTextCoord, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

/*
* Draw the quad from binding the VBOs and drawing it using triangle fans
*/
void Quad::drawQuad() 
{
	/* Bind Quad vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, quadBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind Quad colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, quadColourBO);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind Quad texture coords. Note that this is in attribute index attribute_v_textures */
	glBindBuffer(GL_ARRAY_BUFFER, quadTextBO);
	glEnableVertexAttribArray(attribute_v_textures);
	glVertexAttribPointer(attribute_v_textures, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw points
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

/*
* Clear the vertex buffers of the quad
*/
void Quad::clearQuad() 
{
	// clear the vertex position vector
	quad.clear();

	// add a single value to avoid memory exceptions
	quad.push_back(0);

	/* Create Vertex Buffer for Quad to be empty (i.e. deleting the quad) */
	glGenBuffers(1, &quadBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadBO);
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), &(quad[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}