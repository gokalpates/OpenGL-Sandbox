#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexNormal;
out vec3 vertexFragmentPosition;
out vec2 vertexTextureCoordinates;

void main()
{
	vertexTextureCoordinates = textureCoordinates;

	vertexFragmentPosition = vec3(model * vec4(position, 1.0));

	//Note that this is a very expensive operation. Consider to do that in CPU whenever you scale object.
	vertexNormal = mat3(transpose(inverse(model))) * normal;

	gl_Position = projection * view * vec4(vertexFragmentPosition, 1.0);
}