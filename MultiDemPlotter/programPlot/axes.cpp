

#include "axes.h"

ThreeDAxes::ThreeDAxes()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
}

ThreeDAxes::~ThreeDAxes()
{
}

void ThreeDAxes::makeAxes(int boundaries)
{
	GLfloat xAxesVertex[]
	{
		boundaries + 1, 0.0f, 0.0f,
		-(boundaries + 1), 0.0f, 0.0f
	};

	GLfloat yAxesVertex[]
	{
		0.0f, (boundaries + 1.f), 0.f,
		0.0f, -(boundaries + 1.f), 0.0f
	};

	GLfloat zAxesVertex[]
	{
		0.0f, 0.0f, (boundaries + 1.f),
		0.0f, 0.0f, -(boundaries + 1.f)
	};

	GLfloat xAxesColour[]
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat yAxesColour[]
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	GLfloat zAxesColour[]
	{
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	glGenBuffers(1, &xAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesVertex), xAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &xColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesColour), xAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &yAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesVertex), yAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &yColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesColour), yAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &zAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesVertex), zAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &zColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesColour), zAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ThreeDAxes::drawAxes()
{
	glLineWidth(5.0f);

	// x axes line
	glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// x axes colours
	glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);

	// y axes line
	glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// y axes colours
	glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);

	// z axes line
	glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// z axes colours
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);

}