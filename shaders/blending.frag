#version 330 core

uniform sampler2D texture_diffuse;

in vec2 vertexTexCoords;
out vec4 fragColor;

void main()
{
	fragColor = texture(texture_diffuse,vertexTexCoords);
}