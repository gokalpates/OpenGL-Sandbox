#version 330 core

layout (std140) uniform uBlock
{
	float d1;
	vec3 d2;
	vec2 d3;
	bool d4;
	float d5[2];
	float d6;
};

out vec4 fragColor;

void main()
{
	if((d5[0] == 1.f) && (d5[1] == 2.f) && (d6 == 12.f))
		fragColor = vec4(0.f,1.f,0.f,1.f); //Show box as a green, if buffer is correct.
	else
		fragColor = vec4(1.f,0.f,0.f,1.f); //Show box as a red, if buffer is incorrect. 
}