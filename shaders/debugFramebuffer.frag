#version 330 core

in vec2 vertexTextureCoord;
out vec4 fragColor;

uniform sampler2D debugTexture;

float linearize(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * 0.1 * 1000.0) / (1000.0 + 0.1 - z * (1000.0 - 0.1));
}

void main()
{
	float depth = linearize(texture(debugTexture,vertexTextureCoord).r) / 1000.0;
	fragColor = vec4(vec3(depth), 1.0);
}
