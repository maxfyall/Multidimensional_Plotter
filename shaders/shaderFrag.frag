// Fragment Shader
// Used for everything bar label quads

// Max Fyall - 180011724
// Multidimensional Plotter

#version 420 core // version

// vec4 from vertex shader
in vec4 fragColour;

// colour to use on pixel level
out vec4 outputColour;


void main()
{
	// set pixel colour using colour from vertex shader
	outputColour = fragColour;
}