#version 330 core

in VS_FS
{
	vec2 texCoord;
}fsIn;

out vec4 fragColor;

uniform sampler2D bloomBuffer;
uniform sampler2D hdrBuffer;

uniform bool bloom;
uniform float exposure;

void main()
{
	const float gamma = 2.2;
	
	vec3 hdrColor = texture(hdrBuffer,fsIn.texCoord).rgb;
	vec3 bloomColor = texture(bloomBuffer,fsIn.texCoord).rgb;

	if(bloom)
		hdrColor += bloomColor;

	//Tone mapping.
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	//Gamma correction.
	mapped = pow(mapped, vec3(1.0 / gamma));

	fragColor = vec4(mapped,1.f);
}