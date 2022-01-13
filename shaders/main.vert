#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out VS_FS
{
	vec3 fragPosition;
	vec3 normal;
	vec2 texCoord;
}vsOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vsOut.fragPosition = vec3(model * vec4(position,1.0));

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vsOut.normal = normalize(normalMatrix * normal);

	vsOut.texCoord = texCoord;
	gl_Position = projection * view * vec4(vsOut.fragPosition, 1.0);;
}