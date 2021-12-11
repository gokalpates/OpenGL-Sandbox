#version 330 core

in vec3 vertexNormal;
in vec3 vertexFragmentPosition;
in vec2 vertexTextureCoordinates;

out vec4 fragmentColor;

uniform vec3 viewPosition;
uniform float far;

// Directional Shadow.
in vec4 vertexFragmentPositionLightSpace;
uniform sampler2D directionalShadowMap;

// Point Shadow.
uniform samplerCube pointShadowMap;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
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

float calculateDirectionalShadow(vec4 lightSpaceFragmentPosition);
float calculatePointShadow(PointLight light, vec3 fragmentPosition);

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

	//Gamma Correction.
	result = pow(result,vec3(1.0/2.2));

	//Equals to final color of fragment.
	fragmentColor = vec4(result, 1.0);
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	// Ambient lightning calculation.
	vec3 ambient = light.ambient * texture(material.diffuse0, vertexTextureCoordinates).rgb;

	// Diffuse lightning calculation.
	vec3 lightDirection = normalize(-light.direction);

	float diffValue = max(dot(normal,lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diffValue * texture(material.diffuse0, vertexTextureCoordinates).rgb;

	// Specular lightning calculation.
	float specValue = 0.f;
	vec3 halfwayDirection = normalize(viewDirection + lightDirection);
	specValue = pow(max(dot(normal,halfwayDirection), 0.0), material.shininess * 4.f);

	vec3 specular = light.specular * specValue * texture(material.specular0, vertexTextureCoordinates).rgb;

	// Shadow calculation.
	float shadow = calculateDirectionalShadow(vertexFragmentPositionLightSpace);
	float inverseShadow = (1.0 - shadow);

	// Apply shadow to diffuse and specular light components.
	diffuse *= inverseShadow;
	specular *= inverseShadow;

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
	vec3 ambient = light.ambient * texture(material.diffuse0, vertexTextureCoordinates).rgb;
	ambient *= attenuation;

	//Diffuse light calculation.
	vec3 lightDirection = normalize(light.position - vertexFragmentPosition);

	float diffValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diffValue * texture(material.diffuse0, vertexTextureCoordinates).rgb;
	diffuse *= attenuation;

	//Specular light calculation.
	float specValue = 0.f;
	vec3 halfwayDirection = normalize(viewDirection + lightDirection);
	specValue = pow(max(dot(normal,halfwayDirection), 0.0), material.shininess * 4.f);

	vec3 specular = light.specular * specValue * texture(material.specular0, vertexTextureCoordinates).rgb;
	specular *= attenuation;

	//Shadow calculation.
	float shadow = calculatePointShadow(light,vertexFragmentPosition);
	float inverseShadow = (1.0 - shadow);

	//Aplly shadow to diffuse and specular light components.
	diffuse *= inverseShadow;
	specular *= inverseShadow;

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
	vec3 ambient = light.ambient * texture(material.diffuse0,vertexTextureCoordinates).rgb;

	//Diffuse light calculation.
	vec3 lightDirection = normalize(light.position - vertexFragmentPosition);

	float diffValue = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diffValue * texture(material.diffuse0,vertexTextureCoordinates).rgb;

	//Specular light calculation.
	float specValue = 0.f;
	vec3 halfwayDirection = normalize(viewDirection + lightDirection);
	specValue = pow(max(dot(normal,halfwayDirection), 0.0), material.shininess * 4.f);
	
	vec3 specular = light.specular * specValue * texture(material.specular0,vertexTextureCoordinates).rgb;

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

float calculateDirectionalShadow(vec4 lightSpaceFragmentPosition)
{
	//Perform perspective divide.
	vec3 NDC = lightSpaceFragmentPosition.xyz / lightSpaceFragmentPosition.w;

	//Transform NDC to [0,1] interval for sample shadow map.
	NDC = NDC * 0.5 + 0.5;

	//Closest fragment depth.
	float closestFragmentDepth = texture(directionalShadowMap,NDC.xy).r;

	//Current fragment depth.
	float currentFragmentDepth = NDC.z;

	// Z value clamping.
	if(NDC.z > 1.f) 
		return 0.0;

	//Percentage-Closer Filtering and biasing.
	float shadow = 0.f;
	float bias = 0.0005f;
	vec2 texelOffset = 1.0 / textureSize(directionalShadowMap, 0);

	for(int y = -1; y < 2; y++)
	{
		for(int x = -1; x < 2; x++)
		{
			float pcfDepth = texture(directionalShadowMap,NDC.xy + vec2(x,y) * texelOffset).r;
			shadow += (currentFragmentDepth > pcfDepth + bias ? 1.0 : 0.0);
		}
	}
	
	shadow /= 9.f;
	return shadow;
}

float calculatePointShadow(PointLight light, vec3 fragmentPosition)
{
	//Sampling direction.
	vec3 fragToLight = fragmentPosition - light.position;

	//Closest fragment depth.
	float closestFragmentDepth = texture(pointShadowMap,fragToLight).r;
	closestFragmentDepth *= far; //Convert it back to [0,far] interval.

	//Current fragment depth.
	float currentFragmentDepth = length(fragToLight);

	//Percentage-Closer Filtering and biasing.
	float shadow = 0.f;
	float bias = 0.0005f;
	float interval = 0.1f;
	float sampleCount = 4.f;
	float incrementValue = interval / (sampleCount * 0.5f);
	for(float x = -interval; x < interval; x+=incrementValue)
	{
		for(float y = -interval; y < interval; y+=incrementValue)
		{
			for(float z = -interval; z < interval; z += incrementValue)
			{
				closestFragmentDepth = texture(pointShadowMap, fragToLight + vec3(x,y,z)).r;
				closestFragmentDepth *= far;
				if(currentFragmentDepth > closestFragmentDepth + bias)
					shadow += 1.f;
			}
		}
	}

	shadow /= pow(sampleCount,3);
	return shadow;
}
