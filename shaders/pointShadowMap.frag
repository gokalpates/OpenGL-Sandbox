#version 330 core

in vec4 fragPos;

uniform vec3 lightPosition;
uniform float far;

void main()
{
	vec3 direction = fragPos.xyz - lightPosition;
	float depthValue = length(direction) / far; //Depth values between [0,1].
	gl_FragDepth = depthValue;
}