// Vertex Shader

#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

uniform mat4 model, view, projection;
uniform uint colourMode;

out vec2 TexCoords;
out vec4 fragColour;

void main()
{

	//gl_Position = (projection* view * model) * position;

	gl_Position = projection * vec4(position.xyz, 1.0);

	TexCoords = position.zw;

}