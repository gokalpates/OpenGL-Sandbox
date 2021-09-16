#version 330 core

in VS_FS
{
	vec4 color;
};

out vec4 fragColor;

void main()
{
	fragColor = color;
}