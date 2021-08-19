#version 330 core

uniform sampler2D screenTexture;

in vec2 vertexTex;
out vec4 fragColor;

void main()
{
	fragColor = texture(screenTexture, vertexTex);
}