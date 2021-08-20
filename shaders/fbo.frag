#version 330 core

uniform sampler2D screenTexture;

in vec2 vertexTex;
out vec4 fragColor;

const float offset = 1.f / 1000.f;

void main()
{
	vec2 offsets[9] = {
		vec2(-offset, offset), //Top Left
		vec2(0.0, offset), //Top Center
		vec2(offset,offset), //Top Right
		vec2(-offset,0.0), //Mid Left
		vec2(0.0, 0.0), //Mid Center
		vec2(offset, 0.0), //Mid Right
		vec2(-offset, -offset), //Bottom Left
		vec2(0.0, -offset), //Bottom Center
		vec2(offset, -offset) //Bottom Right
	};

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    );

	vec3 samples[9];
	for(int i = 0; i < 9; i++)
		samples[i] = texture(screenTexture,vertexTex + offsets[i]).rgb;

	vec3 color = vec3(0.0);
	for (int i = 0; i < 9; i++)
		color += samples[i] * kernel[i];

	fragColor = vec4(color,1.0);
}