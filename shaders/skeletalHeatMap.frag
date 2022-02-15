#version 330 core

in VS_FS
{
	vec2 texCoords;
	vec3 normal;
	vec3 localePosition;
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
	vec3 diffuse = texture(material.diffuse0,fsIn.texCoords).rgb;
	diffuse = pow(diffuse,vec3(1.f/2.2f));
	fragColor = vec4(diffuse,1.f);
}