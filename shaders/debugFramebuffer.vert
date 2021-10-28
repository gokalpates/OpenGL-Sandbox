#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoord;

out vec2 vertexTextureCoord;

void main()
{
	vertexTextureCoord = textureCoord;
	gl_Position = vec4(position,0.f,1.f);
}
