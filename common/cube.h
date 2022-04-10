/* cube.h
Example class to to show a cube implementation
Iain Martin November 2018

EXTENDED BY Max Fyall - 180011724
Multidimensional Plotter - 2022
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

// cube class
class Cube
{
public:
	Cube();
	~Cube();

	// functions
	void makeCube(float height, int moveX, int moveZ, static float colour[4]);
	void editColour(static float colour[4]);
	void drawCube(int drawmode);
	void clearCube();

	// Define vertex buffer object names (e.g as globals)
	GLuint positionBufferObject;
	GLuint colourObject;
	GLuint normalsBufferObject;

	GLuint attribute_v_coord;
	GLuint attribute_v_colours;

	int numvertices;

};
