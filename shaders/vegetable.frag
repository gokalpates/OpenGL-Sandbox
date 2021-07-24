#version 330 core

uniform sampler2D texture_diffuse;

in vec2 vertexTexCoordinates;

out vec4 fragColor;

void main()
{
	vec4 colors = texture(texture_diffuse,vertexTexCoordinates);
	if(colors.a < 0.3)
		discard;
	fragColor = colors;
}