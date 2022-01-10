#version 330 core

out vec4 fragColor;

in VS_FS
{
	vec3 fragPosition;
	vec3 normal;
	vec2 texCoord;
}fsIn;

uniform vec3 viewPosition;
uniform vec3 lightPosition;

struct Material
{
	sampler2D diffuse0;
};

uniform Material material;

void main()
{
	vec3 color = texture(material.diffuse0, fsIn.texCoord).rgb;

	vec3 ambient = color * 0.02f;

	vec3 lightDir = normalize(lightPosition - fsIn.fragPosition).rgb;
	vec3 normal = -normalize(fsIn.normal);
	float attenuation = (1.f / length(lightPosition - fsIn.fragPosition));

	float diff = max(dot(lightDir,normal),0.f);
	vec3 diffuse = diff * color * attenuation * 50.f;
	
	vec3 viewDir = normalize(viewPosition - fsIn.fragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal,halfwayDir),0.f),32.f);
	
	vec3 specular = vec3(1.f) * spec * attenuation * vec3(0.f);
	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result,1.0);
}
