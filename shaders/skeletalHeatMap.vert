#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

out VS_FS
{
	vec2 texCoords;
	vec3 normal;
	vec3 localePosition;
	flat ivec4 boneIds;
	vec4 weights;
}vsOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position , 1.f);
	vsOut.texCoords = texCoords;
	vsOut.normal = normal;
	vsOut.localePosition = position;
	vsOut.boneIds = boneIds;
	vsOut.weights = weights;
}