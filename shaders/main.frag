#version 330 core

out vec4 fragmentColor;

in VS_FS
{
	vec2 texCoord;
}fsIn;

uniform vec3 viewPosition;

struct PointLight
{
	vec3 position;
	vec3 color;
};

uniform PointLight pointLights[5];

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;

void main()
{
	vec3 fragPosition = texture(position,fsIn.texCoord).rgb;
	vec3 normal = texture(normal,fsIn.texCoord).rgb;
	vec3 diffuseSample = texture(albedo,fsIn.texCoord).rgb;
	float specularSample = texture(albedo,fsIn.texCoord).a;

	vec3 result = vec3(0.f);

	//Calculate ambient light.
	vec3 ambient = diffuseSample * 0.1f;
	result += ambient;

	//Calculate diffuse and specular light.
	vec3 viewDirection = normalize(viewPosition - fragPosition);
	for(int i = 0; i < 5; i++)
	{
		vec3 lightDirection = normalize(pointLights[i].position - fragPosition);
		float diffuseAngle = max(dot(lightDirection, normal),0.0);
		vec3 diffuse = pointLights[i].color * diffuseAngle * diffuseSample;

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularAngle =  pow(max(dot(normal, halfwayDirection), 0.0), 32.f);
		vec3 specular = pointLights[i].color * specularAngle * specularSample;

		//Attenuation effect.
		float distance = length(fragPosition - pointLights[i].position);
		float attenuation = 1.f / (distance * distance);

		diffuse *= attenuation;
		specular *= attenuation;

		vec3 resultPerLight = diffuse + specular;
		result += resultPerLight;
	}
	
	result = pow(result,vec3(1.f/2.2f));
	fragmentColor = vec4(result, 1.f);
}
