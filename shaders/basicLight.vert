#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_FS
{
	vec3 fragPosition;
	vec2 textureCoords;
	vec3 normal;
	mat3 iTBN;
} vsOut;

void main()
{
	//Inverse TBN calculation.
	vec3 n = mat3(transpose(inverse(model))) * normal;
	vec3 T = normalize(vec3(model * vec4(tangent, 0.f)));
	vec3 N = normalize(vec3(model * vec4(n,0.f)));
	T = normalize(T-dot(T,N) *N);
	vec3 B = cross(N,T);
	vsOut.iTBN = transpose(mat3(T,B,N));

	vsOut.fragPosition = vec3(model * vec4(position, 1.0));
	vsOut.textureCoords = textureCoords;
	vsOut.normal = n;
	gl_Position = projection * view * vec4(vsOut.fragPosition, 1.0);
}
