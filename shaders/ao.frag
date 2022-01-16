#version 330 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noise;

uniform vec3 samples[64]; //64 different sample points.

int kernelSize = 64; //Sample point count.
float radius = 0.5f; //Kernel hemisphere radius.
float bias = 0.025f;

const vec2 noiseScale = vec2(2560.f/4.f,1440.f/4.f);

uniform mat4 projection;

void main()
{
	//In view space.
	vec3 fragPos = texture(gPosition,TexCoords).xyz;
	vec3 normal = normalize(texture(gNormal,TexCoords).xyz);
	vec3 randomVec = normalize(texture(noise, TexCoords * noiseScale).xyz);

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.f;
	for(int i = 0; i < kernelSize; i++)
	{
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample from gPosition.

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; 
	}
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}