#version 330 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 textureCoordinate;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 model;

//Interface block.
out vertexOut
{
	vec2 textureCoordinate;
} vsOut;

void main()
{
	vsOut.textureCoordinate = textureCoordinate;
	gl_Position = projection * view * model * vec4(position, 1.0);
}