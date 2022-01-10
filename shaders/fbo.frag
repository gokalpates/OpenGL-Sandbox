#version 330 core

in VS_FS
{
	vec2 texCoord;
}fsIn;

out vec4 fragColor;

uniform sampler2D hdrColorBuffer;

void main()
{
	vec3 color = texture(hdrColorBuffer,fsIn.texCoord).rgb;

	vec3 reinhard = color / (color + vec3(1.0));
	reinhard = pow(reinhard,vec3(1.f/2.2f));

	fragColor = vec4(reinhard,1.0);
}