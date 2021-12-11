#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_FS
{
	vec3 fragPosition;
	vec3 normal;
	vec2 textureCoords;
} vsOut;

void main()
{
	vsOut.fragPosition = vec3(model * vec4(position, 1.0));
	vsOut.normal = mat3(transpose(inverse(model))) * normal; //Check here because normalize cause errors.
	vsOut.textureCoords = textureCoords;
	gl_Position = projection * view * vec4(vsOut.fragPosition, 1.0);
}
