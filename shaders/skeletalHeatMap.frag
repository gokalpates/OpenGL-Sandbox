#version 330 core

in VS_FS
{
	vec2 texCoords;
	vec3 normal;
	flat ivec4 boneIds;
	vec4 weights;
}fsIn;

out vec4 fragColor;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
};

uniform Material material;

void main()
{
	fragColor = texture(material.diffuse0, fsIn.texCoords);
}