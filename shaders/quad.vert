#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoords;

out vec2 tCoords;

void main()
{
	tCoords = textureCoords;
	gl_Position = vec4(position, 1.f);
}