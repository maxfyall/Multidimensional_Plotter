/*
  3D Axes Class Header File - Create 3D axes from 3 lines

  Max Fyall - 180011724
  Multidimensional Plotter
*/

#pragma once


#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

#include <iostream>

// 3D axes class
class ThreeDAxis
{
public:
	ThreeDAxis();
	~ThreeDAxis();

	// functions
	std::vector<std::string> makeAxis(int boundaries, int bar);
	void drawAxis();
	void clearLabels();

	// define globals
	std::vector<float> labelPosX;
	std::vector<float> labelPosY;
	std::vector<float> labelPosZ;

	GLuint xAxisBufferObject;
	GLuint xColourBuffer;
	GLuint yAxisBufferObject;
	GLuint yColourBuffer;
	GLuint zAxisBufferObject;
	GLuint zColourBuffer;

	GLuint labelXBO;
	GLuint labelYBO;
	GLuint labelZBO;

	GLuint labelColourBuffer;

	GLuint attribute_v_coord;
	GLuint attribute_v_colours;
	float numOfLines;

};
