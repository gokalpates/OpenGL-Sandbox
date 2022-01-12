#version 330 core

in VS_FS
{
	vec2 texCoord;
}fsIn;

out vec4 fragColor;

uniform sampler2D hdrColorBuffer;
uniform sampler2D blurColorBuffer;
uniform float exposure;

void main()
{
	vec3 color = texture(hdrColorBuffer,fsIn.texCoord).rgb;
	vec3 bloomColor = texture(blurColorBuffer,fsIn.texCoord).rgb;
	color += bloomColor;

	vec3 mapped = vec3(1.0) - exp(-color * exposure);
	mapped = pow(mapped, vec3(1.f/2.2f));

	fragColor = vec4(mapped, 1.0);
}