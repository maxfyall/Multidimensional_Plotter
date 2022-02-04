// Vertex Shader

#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

void main()
{
	gl_Position = position;
}