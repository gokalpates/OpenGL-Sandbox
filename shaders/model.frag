#version 330 core

in vertexOut
{
	vec2 tc; //Name can be different via interface blocks. But order must match.
} fsIn;

out vec4 fragColor;

struct Material {
	sampler2D diffuse0;
};

uniform Material material;

void main()
{																		
	fragColor = vec4(texture(material.diffuse0, fsIn.tc).rgb,1.0);
}