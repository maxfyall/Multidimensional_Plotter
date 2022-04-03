
#include "quad.h"

Quad::Quad() 
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	attribute_v_textures = 2;
}

Quad::~Quad() 
{

}

void Quad::makeQuad(float bump, bool small, int dir, float yBump)
{

	std::vector<float> quad;

	if (dir == 0)
	{
		if (!small)
		{
			quad.insert(quad.end(), { (0.0f + bump), 0.1f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.1f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.3f, 0.f });
			quad.insert(quad.end(), { (0.0f + bump), 0.3f, 0.f });
		}
		else
		{
			quad.insert(quad.end(), { (0.0f + bump), 0.2f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.2f, 0.f });
			quad.insert(quad.end(), { (0.09f + bump), 0.21f, 0.f });
			quad.insert(quad.end(), { (0.0f + bump), 0.21f, 0.f });
		}
	}
	else if(dir == 1)
	{
		if (!small)
		{
			quad.insert(quad.end(), { (0.09f+yBump), (0.2f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.2f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.4f+bump), 0.f });
			quad.insert(quad.end(), { (0.09f+yBump), (0.4f+bump), 0.f });
		}
		else
		{
			quad.insert(quad.end(), { (0.09f+yBump), (0.3f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.3f+bump), 0.f });
			quad.insert(quad.end(), { (0.18f+yBump), (0.31f+bump), 0.f });
			quad.insert(quad.end(), { (0.09f+yBump), (0.31f+bump), 0.f });
		}
	}
	else if (dir == 2) 
	{
		if (!small)
		{
			quad.insert(quad.end(), { 0.0f, 0.1f, (0.f+ bump) });
			quad.insert(quad.end(), { 0.0f, 0.1f, (0.09f + bump) });
			quad.insert(quad.end(), { 0.0f, 0.3f, (0.09f + bump) });
			quad.insert(quad.end(), { 0.0f, 0.3f, (0.f + bump) });
		}
		else
		{
			quad.insert(quad.end(), { 0.0f, 0.2f, (0.f+bump) });
			quad.insert(quad.end(), { 0.f, 0.2f, (0.09f+bump) });
			quad.insert(quad.end(), { 0.f, 0.21f, (0.09f+bump) });
			quad.insert(quad.end(), { 0.0f, 0.21f, (0.f+bump) });
		}
	}

	GLfloat quadColour[] =
	{
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0

	};

	GLfloat quadTextCoord[] =
	{
		0.0f, 1.0f, 0,
		1.0f, 1.0f, 0,
		1.0f, 0.0f, 0,
		0.0f, 0.0f, 0,
	};


	glGenBuffers(1, &quadBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadBO);
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), &(quad[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &quadColourBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadColourBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColour), quadColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &quadTextBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTextBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextCoord), quadTextCoord, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//std::cout << "QUAD MADE " << bump << std::endl;

}

void Quad::drawQuad() 
{

	glBindBuffer(GL_ARRAY_BUFFER, quadBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, quadColourBO);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, quadTextBO);
	glEnableVertexAttribArray(attribute_v_textures);
	glVertexAttribPointer(attribute_v_textures, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//std::cout << "DRAWN" << std::endl;

}