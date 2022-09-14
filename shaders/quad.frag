#version 460 core

in vec2 tCoords;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
	vec4 color = texture2D(tex,tCoords);
	fragColor = color;
}