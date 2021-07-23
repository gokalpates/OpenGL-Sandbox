#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float outline = 0.08;

void main()
{
	gl_Position = projection * view * model * vec4(position + normal * outline,1.0);
}