#include "SpotLight.h"

size_t SpotLight::spotLightCount = 0u;

SpotLight::SpotLight(Shader& shader) :
	spotLightIndex(0u),
	shader(shader),
	interruptedShaderProgramId(0),
	shaderUniformName(""),
	position(glm::vec3(0.f, 0.f, 0.f)),
	direction(glm::vec3(1.f,0.f,0.f)),
	ambient(glm::vec3(0.08f, 0.08f, 0.08f)),
	diffuse(glm::vec3(0.7f, 0.7f, 0.7f)),
	specular(glm::vec3(1.f, 1.f, 1.f)),
	linear(0.09f),
	quadratic(0.032f),
	innerCutoff(12.5f),
	outerCutoff(17.5f)
{
	this->setShader(shader);
	spotLightIndex = spotLightCount;
	this->createShaderUniformName();
	incrementSpotLightCount();

	this->updateAllUniforms();
}

SpotLight::~SpotLight()
{
}

void SpotLight::updateAllUniforms()
{
	detectCurrentShaderProgram();
	std::string uniformName = "";

	float inner, outer;
	inner = glm::cos(glm::radians(innerCutoff));
	outer = glm::cos(glm::radians(outerCutoff));

	this->shader.use();
	uniformName = this->addToShaderUniformName(".position");
	this->shader.setVec3(uniformName.c_str(), position);

	uniformName = this->addToShaderUniformName(".ambient");
	this->shader.setVec3(uniformName.c_str(), ambient);

	uniformName = this->addToShaderUniformName(".diffuse");
	this->shader.setVec3(uniformName.c_str(), diffuse);

	uniformName = this->addToShaderUniformName(".specular");
	this->shader.setVec3(uniformName.c_str(), specular);

	uniformName = this->addToShaderUniformName(".linear");
	this->shader.setFloat(uniformName.c_str(), linear);

	uniformName = this->addToShaderUniformName(".quadratic");
	this->shader.setFloat(uniformName.c_str(), quadratic);

	uniformName = this->addToShaderUniformName(".innerCutoff");
	this->shader.setFloat(uniformName.c_str(), inner);

	uniformName = this->addToShaderUniformName(".outerCutoff");
	this->shader.setFloat(uniformName.c_str(), outer);

	useInterruptedShaderProgram();
}

void SpotLight::setPosition(glm::vec3 position)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".position");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), position);
	useInterruptedShaderProgram();

	this->position = position;
}

void SpotLight::setDirection(glm::vec3 direction)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".direction");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), direction);
	useInterruptedShaderProgram();

	this->direction = direction;
}

void SpotLight::setAmbient(glm::vec3 ambient)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".ambient");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), ambient);
	useInterruptedShaderProgram();

	this->ambient = ambient;
}

void SpotLight::setDiffuse(glm::vec3 diffuse)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".diffuse");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), diffuse);
	useInterruptedShaderProgram();

	this->diffuse = diffuse;
}

void SpotLight::setSpecular(glm::vec3 specular)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".specular");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), specular);
	useInterruptedShaderProgram();

	this->specular = specular;
}

void SpotLight::setLinear(float linear)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".linear");

	this->shader.use();
	this->shader.setFloat(uniformName.c_str(), linear);
	useInterruptedShaderProgram();

	this->linear = linear;
}

void SpotLight::setQuadratic(float quadratic)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".quadratic");

	this->shader.use();
	this->shader.setFloat(uniformName.c_str(), quadratic);
	useInterruptedShaderProgram();

	this->quadratic = quadratic;
}

void SpotLight::setInnerCutoff(float innerCutoff)
{
	float value = glm::cos(glm::radians(innerCutoff));

	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".innerCutoff");

	this->shader.use();
	this->shader.setFloat(uniformName.c_str(), value);
	useInterruptedShaderProgram();

	this->innerCutoff = value;
}

void SpotLight::setOuterCutoff(float outerCutoff)
{
	float value = glm::cos(glm::radians(outerCutoff));

	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".outerCutoff");

	this->shader.use();
	this->shader.setFloat(uniformName.c_str(), value);
	useInterruptedShaderProgram();

	this->outerCutoff = value;
}

std::string SpotLight::getShaderUniformName() const
{
	return shaderUniformName;
}

size_t SpotLight::getLightIndex() const
{
	return spotLightIndex;
}

size_t SpotLight::getLightCount()
{
	return spotLightCount;
}

glm::vec3 SpotLight::getPosition() const
{
	return position;
}

glm::vec3 SpotLight::getDirection() const
{
	return direction;
}

glm::vec3 SpotLight::getAmbient() const
{
	return ambient;
}

glm::vec3 SpotLight::getDiffuse() const
{
	return diffuse;
}

glm::vec3 SpotLight::getSpecular() const
{
	return specular;
}

float SpotLight::getLinear() const
{
	return linear;
}

float SpotLight::getQuadratic() const
{
	return quadratic;
}

float SpotLight::getInnerCutoff() const
{
	return innerCutoff;
}

float SpotLight::getOuterCutoff() const
{
	return outerCutoff;
}

void SpotLight::setShader(Shader& shader)
{
	this->shader = shader;
}

void SpotLight::incrementSpotLightCount() const
{
	spotLightCount += 1;
}

void SpotLight::reduceSpotLightCount() const
{
	spotLightCount -= 1;
}

void SpotLight::createShaderUniformName()
{
	std::string temp;
	size_t index = this->getLightIndex();

	temp = "spotLights[" + std::to_string(index) + "]";

	shaderUniformName = temp;
}

std::string SpotLight::addToShaderUniformName(std::string stringExtension) const
{
	return shaderUniformName + stringExtension;
}

void SpotLight::detectCurrentShaderProgram()
{
	glGetIntegerv(GL_CURRENT_PROGRAM, &interruptedShaderProgramId);
}

void SpotLight::useInterruptedShaderProgram()
{
	glUseProgram(interruptedShaderProgramId);
}
