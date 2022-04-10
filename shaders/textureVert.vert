// Vertex Shader
// Used for texturing quads with characters

// Max Fyall - 180011724
// Multidimensional Plotter

#version 420 core // version

// vertex attributes
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 textCoord;

// uniform matricies
uniform mat4 model, view, projection;

// variables to send to the fragment shader
out vec2 TexCoords;
out vec4 fragColour;

void main()
{
	// set colour to vertex attribute
	fragColour = colour;

	// model view calculations
	gl_Position = (projection* view * model) * position;

	// set teture coordinates as vertex attribute coords x and y
	TexCoords = textCoord.xy;

}