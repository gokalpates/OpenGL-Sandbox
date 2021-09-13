#version 330 core

in GS_FS
{
	vec2 textureCoordinate;
} fsIn;

out vec4 fragColor;

struct Material
{
	sampler2D diffuse0;
};

uniform Material material;

void main()
{
	fragColor = texture(material.diffuse0, fsIn.textureCoordinate);
}