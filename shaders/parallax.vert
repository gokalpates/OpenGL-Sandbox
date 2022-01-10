#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinate;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 u_lightPosition;
uniform vec3 u_viewPosition;

out VS_FS
{
	vec3 fragmentPosition;
	vec2 textureCoordinate;
	vec3 tangentLightPosition;
	vec3 tangentViewPosition;
	vec3 tangentFragPosition;
} vsOut;

void main()
{
	gl_Position = projection * view * model * vec4(position,1.0);
	vsOut.fragmentPosition = vec3(model * vec4(position, 1.0));
	vsOut.textureCoordinate = textureCoordinate;

	vec3 T = normalize(mat3(model) * tangent);
	vec3 B = normalize(mat3(model) * bitangent);
	vec3 N = normalize(mat3(model) * normal);
	mat3 iTBN = transpose(mat3(T,B,N));

	vsOut.tangentFragPosition = iTBN * vsOut.fragmentPosition;
	vsOut.tangentViewPosition = iTBN * u_viewPosition;
	vsOut.tangentLightPosition = iTBN * u_lightPosition;
}
