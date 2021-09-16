#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 offset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_FS
{
	vec4 color;
};

void main()
{   
	vec4 wordPosition = model * vec4(position, 1.0);
	wordPosition += vec4(offset, 0.0);

	float value = gl_InstanceID / 99.f;
	color = vec4(value, value, value, 1.f);

	gl_Position = projection * view * wordPosition;
}