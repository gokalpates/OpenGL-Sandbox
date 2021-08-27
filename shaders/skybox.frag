#version 330 core

in vec3 texCoordinates;
out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
	fragColor = texture(skybox, texCoordinates);
}