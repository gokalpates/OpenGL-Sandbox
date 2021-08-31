#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;

uniform samplerCube skybox;
uniform vec3 viewPosition;

out vec4 fragColor;

void main()
{
	vec3 viewDir = normalize((fragPosition - viewPosition));
	vec3 sampleDir = reflect(viewDir, fragNormal);
	fragColor = texture(skybox, sampleDir);
}