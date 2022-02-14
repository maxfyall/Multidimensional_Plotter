// Fragment Shader

#version 420 core

in vec4 fragColour;

out vec4 outputColour;

void main()
{
	outputColour = fragColour;

	//outputColour = vec4(1.0, 0.0, 0.0, 1.0);
}