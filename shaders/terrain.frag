#version 460 core

in float Height;

out vec4 FragColor;

void main()
{
	FragColor = vec4(Height, Height, Height, 1.f);
}