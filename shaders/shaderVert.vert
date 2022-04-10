// Vertex Shader
// Used for everything bar label quads

// Max Fyall - 180011724
// Multidimensional Plotter

#version 420 core // version

// vertex attributes
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;

// uniform matricies
uniform mat4 model, view, projection;

// vec4 for the fragment shader
out vec4 fragColour;

void main()
{
	// set colour from vertex attribute
	fragColour = colour;
	
	// model view calculations
	gl_Position = (projection* view * model) * position;

}