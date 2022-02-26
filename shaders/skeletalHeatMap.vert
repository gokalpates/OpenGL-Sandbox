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
	flat ivec4 boneIds;
	vec4 weights;
}vsOut;

const int MAX_BONE_COUNT = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[MAX_BONE_COUNT];

void main()
{
	mat4 boneTransform = bones[boneIds[0]] * weights[0];
		 boneTransform += bones[boneIds[1]] * weights[1];
		 boneTransform += bones[boneIds[2]] * weights[2];
		 boneTransform += bones[boneIds[3]] * weights[3];

	vec4 localePosition = boneTransform * vec4(position, 1.0);
	gl_Position = projection * view * model * localePosition;
	
	vsOut.texCoords = texCoords;
	vsOut.normal = normal;
	vsOut.boneIds = boneIds;
	vsOut.weights = weights;
}