// Fragment Shader

#version 420 core

in vec4 fragColour;
in vec2 TexCoords;

out vec4 outputColour;

uniform sampler2D text;
uniform vec3 textColour;

void main()
{
	
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	
	outputColour = fragColour * sampled;

	//outputColour = vec4(1.0, 1.0, 1.0, 1.0);

	//vec4 sampled = texture(text, TexCoords);

	//outputColour = fragColour;

}