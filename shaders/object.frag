#version 330 core

in vec3 vertexNormal;
in vec3 vertexFragmentPosition;

out vec4 fragmentColor;

uniform vec3 viewPosition;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

void main()
{
	//Ambient light calculation.
	vec3 ambient = light.ambient * material.ambient;

	//Diffuse light calculation.
	vec3 normal = normalize(vertexNormal);
	vec3 lightDirection = normalize(light.position - vertexFragmentPosition);

	float diffValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * (diffValue * material.diffuse);

	//Specular light calculation.
	vec3 viewDirection = normalize(viewPosition - vertexFragmentPosition);
	vec3 reflection = reflect(-lightDirection, normal);
	
	float specValue = pow(max(dot(viewDirection,reflection), 0.0), material.shininess);
	vec3 specular = light.specular * (specValue * material.specular);

	//Final light calculation.
	vec3 result = (ambient + diffuse + specular);
	fragmentColor = vec4(result, 1.0);
}