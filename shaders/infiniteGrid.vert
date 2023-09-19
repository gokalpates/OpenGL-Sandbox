#version 460 core

const float gridSize = 100000.f;

uniform mat4 view;
uniform mat4 projection;

layout(location = 0) out vec2 uv;

const vec3 positions[4] = vec3[4] (
	vec3(-1.f, 0.f, -1.f),
	vec3( 1.f, 0.f, -1.f),
	vec3( 1.f, 0.f,  1.f),
	vec3(-1.f, 0.f,  1.f)
);

const int indices[6] = int[6](0,1,2,2,3,0);

void main()
{
	vec3 vpos = positions[indices[gl_VertexID]] * gridSize;
	gl_Position = projection * view * vec4(vpos, 1.f);
	uv.xy = vpos.xz;
}