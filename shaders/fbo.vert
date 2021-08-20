#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 vertexTex;

void main()
{
	vertexTex = aTex;
	gl_Position = vec4(aPos.x,aPos.y, 0.f,1.f);
}