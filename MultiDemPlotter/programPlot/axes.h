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
class ThreeDAxes
{
public:
	ThreeDAxes();
	~ThreeDAxes();

	// functions
	std::vector<std::string> makeAxes(int boundaries);
	void drawAxes();
	void clearLabels();

	// define globals
	std::vector<float> labelPosX;
	std::vector<float> labelPosY;
	std::vector<float> labelPosZ;

	GLuint xAxesBufferObject;
	GLuint xColourBuffer;
	GLuint yAxesBufferObject;
	GLuint yColourBuffer;
	GLuint zAxesBufferObject;
	GLuint zColourBuffer;

	GLuint labelXBO;
	GLuint labelYBO;
	GLuint labelZBO;

	GLuint labelColourBuffer;

	GLuint attribute_v_coord;
	GLuint attribute_v_colours;
	float numOfLines;

};
