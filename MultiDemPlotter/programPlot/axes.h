#pragma once


#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

#include <iostream>

class ThreeDAxes
{
public:
	ThreeDAxes();
	~ThreeDAxes();

	void makeAxes(int boundaries);
	void drawAxes();

	GLuint xAxesBufferObject;
	GLuint xColourBuffer;
	GLuint yAxesBufferObject;
	GLuint yColourBuffer;
	GLuint zAxesBufferObject;
	GLuint zColourBuffer;

	GLuint labelXBO;
	GLuint labelYBO;
	GLuint labelZBO;

	GLuint attribute_v_coord;
	GLuint attribute_v_colours;
	float numOfLines;

};
