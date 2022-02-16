#version 330 core

in VS_FS
{
	vec2 texCoords;
	vec3 normal;
	vec3 localePosition;
	flat ivec4 boneIds;
	vec4 weights;
}fsIn;

out vec4 fragColor;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
};

uniform Material material;
uniform int displayBoneIndex = 0;

void main()
{
	bool found = false;
	for(int i = 0; i < 4; i++)
	{
		if(fsIn.boneIds[i] == displayBoneIndex)
		{
			if(fsIn.weights[i] >= 0.7f)
			{
				fragColor = vec4(1.f,0.f,0.f,0.f) * fsIn.weights[i];
			}
			else if(fsIn.weights[i] >= 0.4f && fsIn.weights[i] <= 0.6f)
			{
				fragColor = vec4(0.f,1.f,0.f,0.f) * fsIn.weights[i];
				
			}
			else if(fsIn.weights[i] >= 0.1f)
			{
				fragColor = vec4(1.f,1.f,0.f,0.f) * fsIn.weights[i];
			}
			found = true;
			break;
		}
	}
	if(!found)
	{
		fragColor = vec4(0.f,0.f,1.f,0.f);
	}
}