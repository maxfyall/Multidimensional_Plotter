// Vertex Shader

#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 textCoord;

uniform mat4 model, view, projection;

out vec2 TexCoords;
out vec4 fragColour;

void main()
{

	fragColour = colour;

	gl_Position = (projection* view * model) * position;

	//gl_Position = projection * vec4(position.xy,0.0, 1.0);

	TexCoords = textCoord.xy;

}