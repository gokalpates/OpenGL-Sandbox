#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 vertexTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vertexTex = aTex;
	gl_Position = projection * view * model * vec4(aPos.x,aPos.y,aPos.z,1.f);
}