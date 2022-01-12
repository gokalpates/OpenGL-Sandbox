#version 330 core

//MRT Multiple render target.
layout (location = 0) out vec4 fragmentColor;
layout (location = 1) out vec4 brightColor;

struct Light
{
	vec3 color;
};

uniform Light light;

void main()
{
	fragmentColor = vec4(light.color * 5.f, 1.0);
	float brightness = dot(fragmentColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = vec4(fragmentColor.rgb,1.0);
	else
		brightColor = vec4(vec3(0.f),1.0);
}