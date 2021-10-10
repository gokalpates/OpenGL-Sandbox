#version 330 core

uniform sampler2D screenTexture;

in vec2 vertexTex;
out vec4 fragColor;

uniform bool isGamma;

void main()
{
	vec4 color = texture(screenTexture, vertexTex);
	if(isGamma == true)
	{
		float gamma = 1.f / 2.2f;
		fragColor.rgb = pow(color.rgb, vec3(gamma));
	}
	else
	{
		fragColor = color;
	}
}