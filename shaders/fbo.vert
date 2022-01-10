#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out VS_FS
{
	vec2 texCoord;
}vsOut;

void main()
{
	vsOut.texCoord = aTex;
	gl_Position = vec4(aPos,1.f);
}