#version 330 core

out vec4 fragmentColor;

struct Light
{
	vec3 color;
};

uniform Light light;

void main()
{
	fragmentColor = vec4(light.color, 1.0);
}