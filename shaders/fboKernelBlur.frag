#version 330 core

in VS_FS
{
	vec2 texCoord;
}fsIn;

out vec4 fragColor;

uniform sampler2D brightBuffer;

void main()
{
	const float offset = 1.f / 2000.f;

	const vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    const float kernel[9] = float[](
        1.f / 16.f, 2.f / 16.f, 1.f / 16.f,
        2.f / 16.f, 4.f / 16.f, 2.f / 16.f,
        1.f / 16.f, 2.f / 16.f, 1.f / 16.f  
    );

    vec3 sampleColors[9];
    
    for(int i = 0; i < 9; i++)
        sampleColors[i] = vec3(texture(brightBuffer,fsIn.texCoord.st + offsets[i]));
    
    vec3 result = vec3(0.f);
   
   for(int i = 0; i < 9; i++)
        result += sampleColors[i] * kernel[i];

    fragColor = vec4(result, 1.f);
}