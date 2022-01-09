#version 330 core

//Shader I/O.
in VS_FS
{
	vec3 fragPosition;
	vec2 textureCoords;
	vec3 normal;
	mat3 iTBN;
} fsIn;

out vec4 fragColor;

//Material related.
struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	sampler2D normal0;
	sampler2D parallax0;
	float shininess;
};

//Light related.
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Scene
{
	vec3 viewPosition;
	bool normalMapEnabled;
	bool parallaxMapEnabled;
};

//Initialise components.
uniform Material material;
uniform Light light;
uniform Scene scene;

vec2 calculateParallaxMap(vec2 texCoords, vec3 tViewDirection);

void main()
{	
	//Result color.
	vec3 result = vec3(0.f);

	vec3 diffuseSample, specularSample, normalSample;

	vec3 normal = vec3(0.f);
	if(scene.normalMapEnabled)
	{
		vec2 sampleCoords = vec2(0.f);
		vec3 tangentViewDirection = normalize(fsIn.iTBN * vec3(scene.viewPosition - fsIn.fragPosition));
		if(scene.parallaxMapEnabled)
		{
			sampleCoords = calculateParallaxMap(fsIn.textureCoords,tangentViewDirection);
			if(sampleCoords.x > 1.0 || sampleCoords.y > 1.0 || sampleCoords.x < 0.0 || sampleCoords.y < 0.0)
				discard;
			diffuseSample = texture(material.diffuse0,sampleCoords).rgb;
			specularSample = texture(material.specular0,sampleCoords).rgb;
			normal = texture(material.normal0,sampleCoords).rgb;
		}
		else
		{
			sampleCoords = fsIn.textureCoords;
			diffuseSample = texture(material.diffuse0,sampleCoords).rgb;
			specularSample = texture(material.specular0,sampleCoords).rgb;
			normal = texture(material.normal0,sampleCoords).rgb;
		}
		normal = (normal * 2.0) - 1.0; //Transform values to [-1,1] interval.

		//Ambient light.
		vec3 ambient = diffuseSample * light.ambient;

		//Diffuse light.
		vec3 tangentLightDirection = normalize(fsIn.iTBN * vec3(light.position - fsIn.fragPosition));
		float diffuseAngleValue = max(dot(tangentLightDirection, normal),0.f);

		vec3 diffuse = diffuseSample * diffuseAngleValue * light.diffuse;

		//Specular light.
		vec3 tangentHalfwayDirection = normalize(tangentLightDirection + tangentViewDirection);

		float specAngleValue = pow(max(dot(normal,tangentHalfwayDirection), 0.f), material.shininess * 4.f);
		vec3 specular = specularSample * specAngleValue * light.specular;

		result = ambient + diffuse + specular;
	}
	else
	{
		diffuseSample = texture(material.diffuse0,fsIn.textureCoords).rgb;
		specularSample = texture(material.specular0,fsIn.textureCoords).rgb;

		normal = fsIn.normal;

		//Ambient light.
		vec3 ambient = diffuseSample * light.ambient;

		//Diffuse light.
		vec3 lightDirection = normalize(light.position - fsIn.fragPosition);
		float diffuseAngleValue = max(dot(lightDirection, normal),0.f);

		vec3 diffuse = diffuseSample * diffuseAngleValue * light.diffuse;

		//Specular light.
		vec3 viewDirection = normalize(scene.viewPosition - fsIn.fragPosition);
		vec3 halfwayDirection = normalize(lightDirection + viewDirection);

		float specAngleValue = pow(max(dot(normal,halfwayDirection), 0.0), material.shininess * 4.f);
		vec3 specular = specularSample * specAngleValue * light.specular;

		result = ambient + diffuse + specular;
	}

	//Gamma correction.
	result = pow(result,vec3(1.0/2.2));

	fragColor = vec4(result,1.0);
}

vec2 calculateParallaxMap(vec2 texCoords, vec3 tViewDirection)
{
	float heightScale = 0.1f;
	float height =  texture(material.parallax0, texCoords).r;     
    return texCoords - tViewDirection.xy * (height * heightScale);
}
