#version 330 core

in vec2 vertexTextureCoordinate;

out vec4 fragColor;

struct Material {
	sampler2D diffuse0;
};

uniform Material material;

void main()
{																		
	fragColor = vec4(texture(material.diffuse0, vertexTextureCoordinate).rgb,1.0);
}