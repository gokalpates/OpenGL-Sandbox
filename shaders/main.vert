#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoordinate;

out VS_FS
{
	vec2 texCoord;
}vsOut;

void main()
{
	vsOut.texCoord = textureCoordinate;
	gl_Position = vec4(position, 1.f);
}