#version 330 core

//MRT (Multiple Render Target)
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo; //RGB component as diffuse A as specular.

in VS_FS
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
}fsIn;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
};

uniform Material material;

void main()
{
	gPosition = fsIn.fragPos;
	gNormal = fsIn.normal;
	gAlbedo.rgb = texture(material.diffuse0, fsIn.texCoords).rgb;
	gAlbedo.a = texture(material.specular0, fsIn.texCoords).g;
}
