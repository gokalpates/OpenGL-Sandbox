#version 330 core

in VS_FS
{
	vec2 texCoord;
}fsIn;

out vec4 fragColor;

uniform sampler2D pairsColorBuffer;
uniform bool horizonal;

void main()
{
	const float weight[5] = {0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216};

	vec2 tex_offset = 1.0 / textureSize(pairsColorBuffer,0);
	vec3 result = texture(pairsColorBuffer,fsIn.texCoord).rgb * weight[0];
	if(horizonal)
	{
		for(int i = 1; i < 5; i++)
		{
			result+= texture(pairsColorBuffer,fsIn.texCoord + vec2(tex_offset.x * i,0.0)).rgb * weight[i];
			result+= texture(pairsColorBuffer,fsIn.texCoord - vec2(tex_offset.x * i,0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; i++)
		{
			result+= texture(pairsColorBuffer,fsIn.texCoord + vec2(tex_offset.y * i,0.0)).rgb * weight[i];
			result+= texture(pairsColorBuffer,fsIn.texCoord - vec2(tex_offset.y * i,0.0)).rgb * weight[i];
		}
	}

	fragColor = vec4(result,1.0);
}