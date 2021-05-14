#include "PointLight.h"

size_t PointLight::pointLightCount = 0u;

PointLight::PointLight(Shader& shader) :
	pointLightIndex(0u),
	shader(shader),
	interruptedShaderProgramId(0),
	shaderUniformName(""),
	position(glm::vec3(0.f, 0.f, 0.f)),
	ambient(glm::vec3(0.02f, 0.02f, 0.02f)),
	diffuse(glm::vec3(0.7f, 0.7f, 0.7f)),
	specular(glm::vec3(1.f, 1.f, 1.f)),
	linear(0.09f),
	quadratic(0.032f)
{
	this->setShader(shader);
	pointLightIndex = pointLightCount;
	this->createShaderUniformName();
	this->incrementPointLightCount();

	this->updateAllUniforms();
}

PointLight::~PointLight()
{
}

void PointLight::updateAllUniforms()
{
	detectCurrentShaderProgram();
	std::string uniformName = "";

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

	useInterruptedShaderProgram();
}

void PointLight::setPosition(glm::vec3 position)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".position");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), position);
	useInterruptedShaderProgram();

	this->position = position;
}

void PointLight::setAmbient(glm::vec3 ambient)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".ambient");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), ambient);
	useInterruptedShaderProgram();

	this->ambient = ambient;
}

void PointLight::setDiffuse(glm::vec3 diffuse)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".diffuse");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), diffuse);
	useInterruptedShaderProgram();

	this->diffuse = diffuse;
}

void PointLight::setSpecular(glm::vec3 specular)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".specular");

	this->shader.use();
	this->shader.setVec3(uniformName.c_str(), specular);
	useInterruptedShaderProgram();

	this->specular = specular;
}

void PointLight::setLinear(float linear)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".linear");

	this->shader.use();
	this->shader.setFloat(uniformName.c_str(), linear);
	useInterruptedShaderProgram();

	this->linear = linear;
}

void PointLight::setQuadratic(float quadratic)
{
	detectCurrentShaderProgram();
	std::string uniformName = this->addToShaderUniformName(".quadratic");

	this->shader.use();
	this->shader.setFloat(uniformName.c_str(), quadratic);
	useInterruptedShaderProgram();

	this->quadratic = quadratic;
}

std::string PointLight::getShaderUniformName() const
{
	return shaderUniformName;
}

size_t PointLight::getLightIndex() const
{
	return pointLightIndex;
}

size_t PointLight::getLightCount()
{
	return pointLightCount;
}

glm::vec3 PointLight::getPosition() const
{
	return position;
}

glm::vec3 PointLight::getAmbient() const
{
	return ambient;
}

glm::vec3 PointLight::getDiffuse() const
{
	return diffuse;
}

glm::vec3 PointLight::getSpecular() const
{
	return specular;
}

float PointLight::getLinear() const
{
	return linear;
}

float PointLight::getQuadratic() const
{
	return quadratic;
}

void PointLight::setShader(Shader& shader)
{
	this->shader = shader;
}

void PointLight::incrementPointLightCount() const
{
	pointLightCount += 1;
}

void PointLight::reducePointLightCount() const
{
	pointLightCount -= 1;
}

void PointLight::createShaderUniformName()
{
	std::string temp;
	size_t index = this->getLightIndex();

	temp = "pointLights[" + std::to_string(index) + "]";

	shaderUniformName = temp;
}

std::string PointLight::addToShaderUniformName(std::string stringExtension) const
{
	return shaderUniformName + stringExtension;
}

void PointLight::detectCurrentShaderProgram()
{
	glGetIntegerv(GL_CURRENT_PROGRAM, &interruptedShaderProgramId);
}

void PointLight::useInterruptedShaderProgram()
{
	glUseProgram(interruptedShaderProgramId);
}
