#include "DirectionalLight.h"

size_t DirectionalLight::directionalLightCount = 0u;

DirectionalLight::DirectionalLight(Shader& shader):
	directionalLightIndex(0u),
	shader(shader),
	interruptedShaderProgramId(0),
	shaderUniformName(""),
	direction(glm::vec3(0.f,1.f,0.f)),
	ambient(glm::vec3(0.02f,0.02f,0.02f)),
	diffuse(glm::vec3(0.7f, 0.7f, 0.7f)),
	specular(glm::vec3(1.f, 1.f, 1.f))
{
	this->setShader(shader);
	directionalLightIndex = directionalLightCount;
	this->createShaderUniformName();
	this->incrementDirectionalLightCount();

	this->updateAllUniforms();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::updateAllUniforms()
{
	detectCurrentShaderProgram();
	std::string uniformName = "";

	this->shader.use();
	uniformName = this->addToShaderUniformName(".direction");
	this->shader.setVec3(uniformName.c_str(), this->direction);

	uniformName = this->addToShaderUniformName(".ambient");
	this->shader.setVec3(uniformName.c_str(), this->ambient);

	uniformName = this->addToShaderUniformName(".diffuse");
	this->shader.setVec3(uniformName.c_str(), this->diffuse);

	uniformName = this->addToShaderUniformName(".specular");
	this->shader.setVec3(uniformName.c_str(), this->specular);

	useInterruptedShaderProgram();
}

void DirectionalLight::setShader(Shader& shader)
{
	this->shader = shader;
}

void DirectionalLight::setDirection(glm::vec3 direction)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".direction");
	
	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), direction);
	useInterruptedShaderProgram();

	this->direction = direction;
}

void DirectionalLight::setAmbient(glm::vec3 ambient)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".ambient");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), ambient);
	useInterruptedShaderProgram();

	this->ambient = ambient;
}

void DirectionalLight::setDiffuse(glm::vec3 diffuse)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".diffuse");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), diffuse);
	useInterruptedShaderProgram();

	this->diffuse = diffuse;
}

void DirectionalLight::setSpecular(glm::vec3 specular)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".specular");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), specular);
	useInterruptedShaderProgram();

	this->specular = specular;
}

std::string DirectionalLight::getShaderUniformName() const
{
	return shaderUniformName;
}

size_t DirectionalLight::getLightIndex() const
{
	return directionalLightIndex;
}

size_t DirectionalLight::getLightCount()
{
	return directionalLightCount;
}

glm::vec3 DirectionalLight::getDirection() const
{
	return direction;
}

glm::vec3 DirectionalLight::getAmbient() const
{
	return ambient;
}

glm::vec3 DirectionalLight::getDiffuse() const
{
	return diffuse;
}

glm::vec3 DirectionalLight::getSpecular() const
{
	return specular;
}

void DirectionalLight::incrementDirectionalLightCount() const
{
	directionalLightCount += 1;
}

void DirectionalLight::reduceDirectionalLightCount() const
{
	directionalLightCount -= 1;
}

void DirectionalLight::createShaderUniformName()
{
	std::string temp;
	size_t index = getLightIndex();

	temp = "directionalLights[" + std::to_string(index) + ']';

	shaderUniformName = temp;
}

std::string DirectionalLight::addToShaderUniformName(std::string stringExtension) const
{
	return shaderUniformName + stringExtension;
}

void DirectionalLight::detectCurrentShaderProgram()
{
	glGetIntegerv(GL_CURRENT_PROGRAM, &interruptedShaderProgramId);
}

void DirectionalLight::useInterruptedShaderProgram()
{
	glUseProgram(interruptedShaderProgramId);
}
