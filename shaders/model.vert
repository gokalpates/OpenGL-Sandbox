#version 330 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 textureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offsets[100];

out VS_FS
{
	vec2 texCoord;
};

void main()
{
	
	texCoord = textureCoordinate;
	//Hope that works.
	vec4 worldPosition = model * vec4(position, 1.0);
	worldPosition = worldPosition + vec4(offsets[gl_InstanceID], 1.0);
	gl_Position = projection * view * worldPosition; 
}