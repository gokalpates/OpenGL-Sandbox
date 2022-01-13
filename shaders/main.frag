#version 330 core

//Multiple Render Target.
layout (location = 0) out vec4 fragmentColor;
layout (location = 1) out vec4 brightColor;

in VS_FS
{
	vec3 fragPosition;
	vec3 normal;
	vec2 texCoord;
}fsIn;

uniform vec3 viewPosition;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
};

uniform Material material;

struct PointLight
{
	vec3 position;
	vec3 color;
};

uniform PointLight pointLights[5];

void main()
{
	vec3 diffuseSample = texture(material.diffuse0, fsIn.texCoord).rgb;
	float specularSample = texture(material.specular0, fsIn.texCoord).r;

	vec3 result = vec3(0.f);

	//Calculate ambient light.
	vec3 ambient = diffuseSample * 0.1f;
	result += ambient;

	//Calculate diffuse and specular light.
	vec3 viewDirection = normalize(viewPosition - fsIn.fragPosition);
	for(int i = 0; i < 5; i++)
	{
		vec3 lightDirection = normalize(pointLights[i].position - fsIn.fragPosition);
		float diffuseAngle = max(dot(lightDirection,fsIn.normal),0.0);
		vec3 diffuse = pointLights[i].color * diffuseAngle * diffuseSample;

		vec3 reflectDirection = reflect(-lightDirection,fsIn.normal);
		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularAngle =  pow(max(dot(fsIn.normal, halfwayDirection), 0.0), material.shininess * 4.f);
		vec3 specular = pointLights[i].color * specularAngle * specularSample;

		vec3 resultPerLight = diffuse + specular;

		//Attenuation effect.
		float distance = length(fsIn.fragPosition - pointLights[i].position);
		float attenuation = 1.f / (distance * distance);
		resultPerLight *= attenuation;

		result += resultPerLight;
	}
	
	fragmentColor = vec4(result, 1.f);
	
	float brightness = dot(fragmentColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = vec4(fragmentColor.rgb, 1.0);
	else
		brightColor = vec4(vec3(0.f),1.f);
}
