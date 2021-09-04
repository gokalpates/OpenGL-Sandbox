#version 330 core

in vec3 vertexNormal;
in vec3 vertexFragmentPosition;
in vec2 vertexTextureCoordinates;

out vec4 fragmentColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float linear;
	float quadratic;

	float innerCutoff;
	float outerCutoff;
};

uniform vec3 viewPosition;
uniform Material material;

#define NUMBER_OF_DIRECTIONAL_LIGHTS 1
uniform DirectionalLight directionalLights[NUMBER_OF_DIRECTIONAL_LIGHTS];

#define NUMBER_OF_POINT_LIGHTS 1
uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS]; 

#define NUMBER_OF_SPOTLIGHTS 1
uniform SpotLight spotLights[NUMBER_OF_SPOTLIGHTS];

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDirection);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection);

void main()
{
	vec3 result = vec3(0.0);

	vec3 normal = normalize(vertexNormal);
	vec3 viewDirection = normalize(viewPosition - vertexFragmentPosition);

	//Result of all directional lights.
	for(int i = 0; i < NUMBER_OF_DIRECTIONAL_LIGHTS; i++)
		result += calculateDirectionalLight(directionalLights[i], normal, viewDirection);

	//Result of all point lights.
	for(int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++)
		result += calculatePointLight(pointLights[i], normal, viewDirection);

	//Result of all spotlights.
	for(int i = 0; i < NUMBER_OF_SPOTLIGHTS; i++)
		result += calculateSpotLight(spotLights[i], normal, viewDirection);

	//Equals to final color of fragment.
	fragmentColor = vec4(result, 1.0);
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	// Ambient lightning calculation.
	vec3 ambient = light.ambient * texture(material.diffuse, vertexTextureCoordinates).rgb;

	// Diffuse lightning calculation.
	vec3 lightDirection = normalize(-light.direction);

	float diffValue = max(dot(normal,lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diffValue * texture(material.diffuse, vertexTextureCoordinates).rgb;

	// Specular lightning calculation.
	vec3 reflection = reflect(-lightDirection, normal);

	float specValue = pow(max(dot(viewDirection,reflection), 0.0), material.shininess);
	vec3 specular = light.specular * specValue * texture(material.specular, vertexTextureCoordinates).rgb;

	// Calculate result and return.
	vec3 result = (ambient + diffuse + specular);

	return result;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
	//Attenuation calculation.
	float distanceValue = length(light.position - vertexFragmentPosition);
	float attenuation = 1.0 / (1.0 + (light.linear * distanceValue) + (light.quadratic * pow(distanceValue, 2)));

	//Ambient light calculation.
	vec3 ambient = light.ambient * texture(material.diffuse, vertexTextureCoordinates).rgb;
	ambient *= attenuation;

	//Diffuse light calculation.
	vec3 lightDirection = normalize(light.position - vertexFragmentPosition);
	
	float diffValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diffValue * texture(material.diffuse, vertexTextureCoordinates).rgb;
	diffuse *= attenuation;

	//Specular light calculation.
	vec3 reflection = reflect(-lightDirection, normal);

	float specValue = pow(max(dot(viewDirection,reflection), 0.0), material.shininess);
	vec3 specular = light.specular * specValue * texture(material.specular, vertexTextureCoordinates).rgb;
	specular *= attenuation;

	//Calculate result and return.
	vec3 result = (ambient + diffuse + specular);
	return result;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection)
{
	//Attenuation calculation.
	float distanceValue = length(light.position - vertexFragmentPosition);
	float attenuation = 1.0 / (1.0 + (light.linear * distanceValue) + (light.quadratic * pow(distanceValue, 2)));

	//Ambient light calculation.
	vec3 ambient = light.ambient * texture(material.diffuse,vertexTextureCoordinates).rgb;

	//Diffuse light calculation.
	vec3 lightDirection = normalize(light.position - vertexFragmentPosition);

	float diffValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diffValue * texture(material.diffuse,vertexTextureCoordinates).rgb;

	//Specular light calculation.
	vec3 reflection = reflect(-lightDirection, normal);
	
	float specValue = pow(max(dot(viewDirection,reflection), 0.0), material.shininess);
	vec3 specular = light.specular * specValue * texture(material.specular,vertexTextureCoordinates).rgb;

	//Softening edges.
	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = (light.innerCutoff - light.outerCutoff);
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
	
	//Apply attenuation and softening.
	ambient *= intensity * attenuation;
	diffuse *= intensity * attenuation;
	specular *= intensity * attenuation;

	vec3 result = (ambient + diffuse + specular);
	return result;
}