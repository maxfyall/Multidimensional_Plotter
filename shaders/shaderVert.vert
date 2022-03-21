// Vertex Shader

#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

uniform mat4 model, view, projection;
uniform uint colourMode;

out vec4 fragColour;

void main()
{
	if(colourMode == 1)
	{
		fragColour = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else if(colourMode == 0)
	{
		fragColour = colour;
		//fragColour = vec4(1.0, 1.0, 1.0, 1.0);
	}

	gl_Position = (projection* view * model) * position;
}