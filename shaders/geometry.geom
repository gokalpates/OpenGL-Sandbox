#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_GS
{
	vec2 textureCoordinate;
} vertex[];

out GS_FS
{
	vec2 textureCoordinate;
} gsOut;

uniform float time;

vec3 getNormal();
vec4 explode(vec4 position, vec3 normal);

void main()
{
	vec3 normal = getNormal();

	gl_Position = explode(gl_in[0].gl_Position, normal);
	gsOut.textureCoordinate = vertex[0].textureCoordinate;
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, normal);
	gsOut.textureCoordinate = vertex[1].textureCoordinate;
	EmitVertex();

	gl_Position = explode(gl_in[2].gl_Position, normal);
	gsOut.textureCoordinate = vertex[2].textureCoordinate;
	EmitVertex();

	EndPrimitive();
}

vec3 getNormal()
{
	vec3 A = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
	vec3 B = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
	return normalize(cross(B,A));
}

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 10.0;
	vec3 direction = normal * ((sin(time) + 1)) * magnitude;
	return position + vec4(direction,0.0);
}