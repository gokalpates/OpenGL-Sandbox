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
	vec3 normal;
	vec2 textureCoords;
	mat3 TBN;
} vsOut;

void main()
{
	vec3 T = normalize(vec3(model * vec4(tangent, 0.f)));
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.f)));
	vec3 N = normalize(vec3(model * vec4(normal,0.f)));
	
	vsOut.TBN = mat3(T,B,N);
	vsOut.fragPosition = vec3(model * vec4(position, 1.0));
	vsOut.normal = mat3(transpose(inverse(model))) * normal; //Check here because normalize cause errors.
	vsOut.textureCoords = textureCoords;
	gl_Position = projection * view * vec4(vsOut.fragPosition, 1.0);
}
