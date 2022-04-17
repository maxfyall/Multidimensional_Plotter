/*
  3D Axes Class - Create 3D axes from 3 lines

  Max Fyall - 180011724
  Multidimensional Plotter
*/

#include "axes.h"

/* Establish public global variables */
ThreeDAxes::ThreeDAxes()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
}

/* Deconstructor not used */
ThreeDAxes::~ThreeDAxes()
{
}

/*
* Create vertex buffers to create 3D axes template
* - Calculates labels required and notchs for where they should go
* - Returns labels required as a vector of strings
*/
std::vector<std::string> ThreeDAxes::makeAxes(int boundaries, int bar)
{

	// check if highest value is less than 0 (this could cause the axes to not draw since the vertex position will be 0)
	if (boundaries < 0 )
	{
		boundaries = abs(boundaries);
	}

	// define vertex positions for the x axes using passed in variable (highest number from data)
	// We add one to so we dont get a 0 value (i.e. if boundaries = 0)
	GLfloat xAxesVertex[]
	{
		(boundaries + 1), 0.0f, 0.0f,
		-(boundaries + 1), 0.0f, 0.0f
	};

	// define vertex positions for the y axes using passed in variable (highest number from data)
	// We add one to so we dont get a 0 value (i.e. if boundaries = 0)
	GLfloat yAxesVertex[]
	{
		0.0f, (boundaries + 1.f), 0.f,
		0.0f, -(boundaries + 1.f), 0.0f
	};

	// define vertex positions for the z axes using passed in variable (highest number from data)
	// We add one to so we dont get a 0 value (i.e. if boundaries = 0)
	GLfloat zAxesVertex[]
	{
		0.0f, 0.0f, (boundaries + 1.f),
		0.0f, 0.0f, -(boundaries + 1.f)
	};

	// define colours for each x axes vertex (RED)
	GLfloat xAxesColour[]
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};

	// define colours for each y axes vertex (BLUE)
	GLfloat yAxesColour[]
	{
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	// define colours for each z axes vertex (GREEN)
	GLfloat zAxesColour[]
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	// define a vector of strings to store the number labels we require for texturing
	std::vector<std::string> numLabels;

	// local variables for defining axes notches
	float multiple;
	int addBY;

	// I set it up so that if the largest value (Boundaries) is bigger than 10, then each axes notch would be in increments of 2 rather than 1
	// However if the largest value is less then 10, then axes notches would be in increments of 1

	if (bar == 1)
	{
		// the number of notches requried is the same as out highest value
		numOfLines = boundaries;

		// set local variables for later use
		multiple = 1;
		addBY = 1;
	}
	else 
	{
		if (boundaries < 10)
		{
			// the number of notches requried is the same as out highest value
			numOfLines = boundaries;

			// set local variables for later use
			multiple = 1;
			addBY = 1;
		}
		else if ((boundaries + 1) % 2) // check if largest value is even
		{
			// set the number of notches to be drawn
			numOfLines = (boundaries + 1) / 2;

			// set local variables for later use
			multiple = 2;
			addBY = 2;
		}
		else // otherwise largest value is odd
		{
			// set the number of notches to be drawn
			numOfLines = (boundaries - 1) / 2;

			// set local variables for later use
			multiple = 2;
			addBY = 2;
		}
	}
	

	// create float vector for label colours
	std::vector<float> labelColour;

	// clear vertex position vector for all label notch positions
	labelPosX.clear();
	labelPosY.clear();
	labelPosZ.clear();

	// if we have no lines to draw
	if (numOfLines == 0)
	{
		// insert a single 0 value to each notch position vector and colour vector to prevent memory exceptions
		labelPosX.push_back(0);
		labelPosY.push_back(0);
		labelPosZ.push_back(0);
		labelColour.push_back(0);
	}
	else // otherwise create vertex positions for the label notches
	{
		// loop for the number of lines we need to draw
		for (int i = 0; i < numOfLines; i++)
		{
			// define vertex positions for a notch on the x axes using pre-defined variable multiple
			labelPosX.insert(labelPosX.end(), { (multiple), 0.1, 0 });
			labelPosX.insert(labelPosX.end(), { (multiple), -0.1, 0 });
			labelPosX.insert(labelPosX.end(), { (-multiple), 0.1, 0 });
			labelPosX.insert(labelPosX.end(), { (-multiple), -0.1, 0 });
			
			// define vertex positions for a notch on the y axes using pre-defined variable multiple
			labelPosY.insert(labelPosY.end(), { 0.1 , (multiple), 0 });
			labelPosY.insert(labelPosY.end(), { -0.1 , (multiple), 0 });
			labelPosY.insert(labelPosY.end(), { 0.1 , (-multiple), 0 });
			labelPosY.insert(labelPosY.end(), { -0.1, (-multiple), 0 });

			// define vertex positions for a notch on the z axes using pre-defined variable multiple
			labelPosZ.insert(labelPosZ.end(), { 0, 0.1, (multiple) });
			labelPosZ.insert(labelPosZ.end(), { 0, -0.1, (multiple) });
			labelPosZ.insert(labelPosZ.end(), { 0, 0.1, -(multiple) });
			labelPosZ.insert(labelPosZ.end(), { 0, -0.1, -(multiple) });

			// add the label to the label vector so we can use it to texture the label later
			// we add a negative value as well since it is mirrored on both sides
			numLabels.push_back(std::to_string(multiple));
			numLabels.push_back(std::to_string(-multiple));

			// increase or variable by pre-defined value addBY
			multiple = multiple + addBY;

		}

		// create colours for each notch vertex position
		for (int i = 0; i < (labelPosX.size() * 3); i++)
		{
			// we make the notches white
			labelColour.insert(labelColour.end(), 1.0f);
		}
	}

	/* Create Vertex Buffer for X Axes */
	glGenBuffers(1, &xAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesVertex), xAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Colour Buffer for X Axes */
	glGenBuffers(1, &xColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxesColour), xAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for Y Axes */
	glGenBuffers(1, &yAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesVertex), yAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Colour Buffer for Y Axes */
	glGenBuffers(1, &yColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yAxesColour), yAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for Z Axes */
	glGenBuffers(1, &zAxesBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesVertex), zAxesVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Colour Buffer for Z Axes */
	glGenBuffers(1, &zColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zAxesColour), zAxesColour, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for X Axes Label Notches */
	glGenBuffers(1, &labelXBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelXBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosX.size() * sizeof(float), &(labelPosX[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for Y Axes Label Notches */
	glGenBuffers(1, &labelYBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelYBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosY.size() * sizeof(float), &(labelPosY[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for Z Axes Label Notches */
	glGenBuffers(1, &labelZBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelZBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosZ.size() * sizeof(float), &(labelPosZ[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Colour Buffer for Axes Label Notches */
	glGenBuffers(1, &labelColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, labelColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, labelColour.size() * sizeof(float), &(labelColour[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// return the vector of strings containing all the labels we need
	return numLabels;
	
}

/*
* Draw the 3D axes and notches from binding the VBOs and drawing use GL_LINES
*/
void ThreeDAxes::drawAxes()
{
	// define thickness of axes lines
	glLineWidth(5.0f);

	/* Bind X Axes vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, xAxesBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind X Axes Colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, xColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// draw vertex positions
	glDrawArrays(GL_LINES, 0, 2);

	/* Bind Y Axes vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, yAxesBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind Y Axes Colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, yColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// draw vertex positions
	glDrawArrays(GL_LINES, 0, 2);

	/* Bind Z Axes vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, zAxesBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind Z Axes Colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, zColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// draw vertex positions
	glDrawArrays(GL_LINES, 0, 2);

	// define thickness of notch lines
	glLineWidth(2.0f);

	/* Bind Axes Notch Colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, labelColourBuffer);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind X Axes Notch vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, labelXBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw vertex positions
	glDrawArrays(GL_LINES, 0, numOfLines*4);

	/* Bind Y Axes Notch vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, labelYBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw vertex positions
	glDrawArrays(GL_LINES, 0, numOfLines*4);

	/* Bind Z Axes Notch vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, labelZBO);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// draw vertex positions
	glDrawArrays(GL_LINES, 0, numOfLines*4);

}

/*
* Clear vertex buffers of labels
*/
void ThreeDAxes::clearLabels() 
{
	// clear label notches vectors
	labelPosX.clear();
	labelPosY.clear();
	labelPosZ.clear();

	// add single value to prevent memory exceptions
	labelPosX.push_back(0);
	labelPosY.push_back(0);
	labelPosZ.push_back(0);

	/* Create Vertex Buffer for X Axes notches to be empty (i.e. deleting the notches) */
	glGenBuffers(1, &labelXBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelXBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosX.size() * sizeof(float), &(labelPosX[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for Y Axes notches to be empty (i.e. deleting the notches) */
	glGenBuffers(1, &labelYBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelYBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosY.size() * sizeof(float), &(labelPosY[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create Vertex Buffer for Z Axes notches to be empty (i.e. deleting the notches) */
	glGenBuffers(1, &labelZBO);
	glBindBuffer(GL_ARRAY_BUFFER, labelZBO);
	glBufferData(GL_ARRAY_BUFFER, labelPosZ.size() * sizeof(float), &(labelPosZ[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}