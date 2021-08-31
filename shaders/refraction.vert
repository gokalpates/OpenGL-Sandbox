#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	fragPosition = vec3(model * vec4(aPos, 1.0));
	fragNormal = mat3(transpose(inverse(model))) * aNormal; // Costly operation. Doing this on CPU side is considerable whenever scale the object.
	gl_Position = projection * view * vec4(fragPosition, 1.0);
}