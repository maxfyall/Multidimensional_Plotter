

#include "axes.h"

ThreeDAxes::ThreeDAxes()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
}

ThreeDAxes::~ThreeDAxes()
{
}

std::vector<std::string> ThreeDAxes::makeAxes(int boundaries)
{
	GLfloat xAxesVertex[]
	{
		(boundaries + 1), 0.0f, 0.0f,
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
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	GLfloat zAxesColour[]
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	std::vector<std::string> numLabels;

	float multiple;
	int addBY;

	if (boundaries < 10)
	{
		numOfLines = boundaries;
		multiple = 1;
		addBY = 1;
	}
	else if ((boundaries + 1) % 2)
	{
		numOfLines = (boundaries + 1) / 2;
		multiple = 2;
		addBY = 2;
	}
	else
	{
		numOfLines = (boundaries - 1) / 2;
		multiple = 2;
		addBY = 2;
	}

	std::vector<float> labelColour;

	std::cout << "Num of Lines :" << numOfLines << std::endl;

	labelPosX.clear();
	labelPosY.clear();
	labelPosZ.clear();

	if (numOfLines == 0)
	{
		labelPosX.push_back(0);
		labelPosY.push_back(0);
		labelPosZ.push_back(0);
		labelColour.push_back(0);
	}
	else 
	{
		for (int i = 0; i < numOfLines; i++)
		{
			labelPosX.insert(labelPosX.end(), { (multiple), 0.1, 0 });
			labelPosX.insert(labelPosX.end(), { (multiple), -0.1, 0 });
			labelPosX.insert(labelPosX.end(), { (-multiple), 0.1, 0 });
			labelPosX.insert(labelPosX.end(), { (-multiple), -0.1, 0 });

			labelPosY.insert(labelPosY.end(), { 0, (multiple), 0.1 });
			labelPosY.insert(labelPosY.end(), { 0, (multiple), -0.1 });
			labelPosY.insert(labelPosY.end(), { 0, (-multiple), 0.1 });
			labelPosY.insert(labelPosY.end(), { 0, (-multiple), -0.1 });

			labelPosZ.insert(labelPosZ.end(), { 0, 0.1, (multiple) });
			labelPosZ.insert(labelPosZ.end(), { 0, -0.1, (multiple) });
			labelPosZ.insert(labelPosZ.end(), { 0, 0.1, -(multiple) });
			labelPosZ.insert(labelPosZ.end(), { 0, -0.1, -(multiple) });

			numLabels.push_back(std::to_string(multiple));
			numLabels.push_back(std::to_string(-multiple));

			multiple = multiple + addBY;

			//std::cout << numberLabels[i] << std::endl;

		}

		for (int i = 0; i < (labelPosX.size() * 3); i++)
		{
			labelColour.insert(labelColour.end(), 1.0f);
		}
	}

	// x axes
	glGenBuffers(1, &xAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesVertex), xAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// x axes colour (RED)
	glGenBuffers(1, &xColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesColour), xAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// y axes
	glGenBuffers(1, &yAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesVertex), yAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// y axse colour (BLUE)
	glGenBuffers(1, &yColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesColour), yAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// z axes
	glGenBuffers(1, &zAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesVertex), zAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// z axes colour (GREEN)
	glGenBuffers(1, &zColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesColour), zAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &labelXBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelXBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosX.size() * sizeof(float), &(labelPosX[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &labelYBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelYBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosY.size() * sizeof(float), &(labelPosY[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &labelZBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelZBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosZ.size() * sizeof(float), &(labelPosZ[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &labelColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, labelColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, labelColour.size() * sizeof(float), &(labelColour[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return numLabels;
	
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
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// z axes colours
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);

	glBindBuffer(GL_ARRAY_BUFFER, labelColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, labelXBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, numOfLines*4);

	glBindBuffer(GL_ARRAY_BUFFER, labelYBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, numOfLines*4);

	glBindBuffer(GL_ARRAY_BUFFER, labelZBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, numOfLines*4);

}

void ThreeDAxes::clearLabels() 
{
	labelPosX.clear();
	labelPosY.clear();
	labelPosZ.clear();

	labelPosX.push_back(0);
	labelPosY.push_back(0);
	labelPosZ.push_back(0);

	glGenBuffers(1, &labelXBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelXBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosX.size() * sizeof(float), &(labelPosX[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &labelYBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelYBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosY.size() * sizeof(float), &(labelPosY[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &labelZBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelZBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosZ.size() * sizeof(float), &(labelPosZ[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}