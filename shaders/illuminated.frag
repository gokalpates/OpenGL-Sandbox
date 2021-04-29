#version 330 core

in vec3 vertexNormal;
in vec3 vertexFragmentPosition;

out vec4 fragmentColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
	//Ambient light calculation.
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	//Diffuse light calculation.
	vec3 normal = normalize(vertexNormal);
	vec3 lightDirection = normalize(lightPosition - vertexFragmentPosition);

	float diffValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diffValue * lightColor;

	//Specular light calculation.
	float specularStrength = 0.5;
	vec3 viewDirection = normalize(viewPosition - vertexFragmentPosition);
	vec3 reflection = reflect(-lightDirection, normal);
	
	float specValue = pow(max(dot(viewDirection,reflection), 0.0), 64);
	vec3 specular = specularStrength * specValue * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragmentColor = vec4(result, 1.0);
}