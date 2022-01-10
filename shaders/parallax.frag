#version 330 core

out vec4 fragmentColor;

in VS_FS
{
	vec3 fragmentPosition;
	vec2 textureCoordinate;
	vec3 tangentLightPosition;
	vec3 tangentViewPosition;
	vec3 tangentFragPosition;
} fsIn;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	sampler2D normal0;
	sampler2D parallax0;
};

uniform Material material;

vec2 parallaxMap(vec2 texCoords, vec3 viewDir);

void main()
{
	vec3 viewDirection = normalize(fsIn.tangentViewPosition - fsIn.tangentFragPosition);
	vec2 sampleCoords = parallaxMap(fsIn.textureCoordinate,viewDirection);
	if(sampleCoords.x > 1.0 || sampleCoords.y > 1.0 || sampleCoords.x < 0.0 || sampleCoords.y < 0.0)
		discard;

	vec3 normal = texture(material.normal0, sampleCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 color = texture(material.diffuse0,sampleCoords).rgb;

	vec3 ambient = 0.1 * color;
	
	vec3 lightDirection = normalize(fsIn.tangentLightPosition - fsIn.tangentFragPosition);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * color;
	
	vec3 reflectDir = reflect(-lightDirection, normal);
    vec3 halfwayDir = normalize(lightDirection + viewDirection);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec;
    fragmentColor = vec4(ambient + diffuse + specular, 1.0);
}

vec2 parallaxMap(vec2 texCoords, vec3 viewDir)
{
	float heightScale = 0.1f;

    float numLayers = 100.f;  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(material.parallax0, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.parallax0, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.parallax0, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
