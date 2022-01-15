#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out VS_FS
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
}vsOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vsOut.fragPos = vec3(model * vec4(position,1.f));
	vsOut.normal = transpose(inverse(mat3(model))) * normal;
	vsOut.texCoords = texCoords;

	gl_Position = projection * view * vec4(vsOut.fragPos,1.f);
}