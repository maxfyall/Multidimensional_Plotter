/*
  Quad Class Header File - Create Quad for texturing characters onto

  Max Fyall - 180011724
  Multidimensional Plotter
*/

#pragma once


#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

#include <iostream>

// Quad class
class Quad 
{
public:
	Quad();
	~Quad();

	// functions
	void makeQuad(float bump, int small, int dir, float yBump);
	void drawQuad();
	void clearQuad();

	// globals
	GLuint quadBO;
	GLuint quadColourBO;
	GLuint quadTextBO;

	GLuint attribute_v_coord;
	GLuint attribute_v_colours;
	GLuint attribute_v_textures;

	std::vector<float> quad;

};
