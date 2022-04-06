/* cube.h
Example class to to show a cube implementation
Iain Martin November 2018
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

class Cube
{
public:
	Cube();
	~Cube();

	void makeCube(float height, int moveX, int moveZ);
	void drawCube(int drawmode);
	void clearCube();

	// Define vertex buffer object names (e.g as globals)
	GLuint positionBufferObject;
	GLuint colourObject;
	GLuint normalsBufferObject;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;

	int numvertices;

};
