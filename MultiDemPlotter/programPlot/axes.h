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

	std::vector<std::string> makeAxes(int boundaries);
	void drawAxes();
	void clearLabels();

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
