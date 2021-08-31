#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;

uniform samplerCube skybox;
uniform vec3 viewPosition;

out vec4 fragColor;

void main()
{
	float reflactionRatio = 1.0 / 2.42; // Air to diamond.
	vec3 viewDir = normalize((fragPosition - viewPosition));
	vec3 sampleDir = refract(viewDir, fragNormal, reflactionRatio);
	fragColor = texture(skybox, sampleDir);
}