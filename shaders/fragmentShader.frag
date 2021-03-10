#version 330 core

in vec3 vertexColor;
in vec2 vertexTextureCoordinates;

out vec4 fragmentColor;

uniform sampler2D texture0;

void main()
{
	fragmentColor = texture(texture0,vertexTextureCoordinates);
}