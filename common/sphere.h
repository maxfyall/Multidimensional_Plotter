/* sphere.h
 Example class to create a generic sphere object
 Resolution can be controlled by setting the number of latitudes and longitudes
 Iain Martin November 2018
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere();
	~Sphere();

	void makeSphere(GLuint numlats, GLuint numlongs);
	void drawSphere(int drawmode);

	// Define vertex buffer object names (e.g as globals)
	GLuint sphereBufferObject;
	GLuint sphereNormals;
	GLuint sphereColours;
	GLuint elementbuffer;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;

	int numspherevertices;
	int numlats;
	int numlongs;

private:
	void makeUnitSphere(GLfloat *pVertices);
};
