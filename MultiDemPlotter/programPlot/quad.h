#pragma once


#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

#include <iostream>

class Quad 
{
public:
	Quad();
	~Quad();

	void makeQuad(float bump, bool small, int dir, float yBump);
	void drawQuad();

	GLuint quadBO;
	GLuint quadColourBO;
	GLuint quadTextBO;

	GLuint attribute_v_coord;
	GLuint attribute_v_colours;
	GLuint attribute_v_textures;

};
