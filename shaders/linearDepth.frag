#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material
{
    sampler2D diffuse0;
};

uniform Material material;

float near = 0.1, far = 100.0;

float linearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{   
    float depth = linearizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depth), 1.0);
}