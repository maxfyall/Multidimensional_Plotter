// Fragment Shader
// Used for texturing quads with characters

// Max Fyall - 180011724
// Multidimensional Plotter

#version 420 core // version

// variables from vertex shader
in vec4 fragColour;
in vec2 TexCoords;

// final colour (combination of texture plus quad colour)
out vec4 outputColour;

// uniform texture variable
uniform sampler2D text;

void main()
{
	// calculate texture
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords));
	
	// combine texture colour with vertex attrib colour
	outputColour = fragColour * sampled;

}