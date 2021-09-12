#version 330 core

in VS_OUT
{
	vec2 texCoord;
};

out vec4 fragColor;

struct Material {
	sampler2D diffuse0;
};

uniform Material material;

void main()
{	
	fragColor = texture(material.diffuse0, texCoord);
}