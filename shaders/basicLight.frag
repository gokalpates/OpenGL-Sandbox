#version 330 core

//Shader I/O.
in VS_FS
{
	vec3 fragPosition;
	vec3 normal;
	vec2 textureCoords;
} fsIn;

out vec4 fragColor;

//Material related.
struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
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
};

//Initialise components.
uniform Material material;
uniform Light light;
uniform Scene scene;

void main()
{	
	//Necessary pre-calculations.
	vec3 diffuseSample = texture(material.diffuse0, fsIn.textureCoords).rgb;
	vec3 specularSample = texture(material.specular0, fsIn.textureCoords).rgb;
	vec3 normal = normalize(fsIn.normal);

	//Ambient light.
	vec3 ambient = diffuseSample * light.ambient;

	//Diffuse light.
	vec3 lightDirection = normalize(light.position - fsIn.fragPosition);
	float diffuseAngleValue = max(dot(lightDirection, normal),0.f);

	vec3 diffuse = diffuseSample * diffuseAngleValue * light.diffuse;

	//Specular light.
	vec3 viewDirection = normalize(scene.viewPosition - fsIn.fragPosition);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float specAngleValue = pow(max(dot(normal,halfwayDirection),0.0), material.shininess);
	vec3 specular = specularSample * specAngleValue * light.specular;

	//Gamma correction.
	vec3 result = ambient + diffuse + specular;
	result = pow(result,vec3(1.0/2.2));

	fragColor = vec4(result,1.0);
}
