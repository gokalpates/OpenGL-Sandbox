#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

class DirectionalLight
{
public:
	DirectionalLight(Shader& shader);
	~DirectionalLight();

	void updateAllUniforms();

	void setDirection(glm::vec3 direction);
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

	std::string getShaderUniformName() const;
	size_t getLightIndex() const;
	static size_t getLightCount();
	glm::vec3 getDirection() const;
	glm::vec3 getAmbient() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;

private:
	void setShader(Shader& shader);
	void incrementDirectionalLightCount() const;
	void reduceDirectionalLightCount() const;
	void createShaderUniformName();
	std::string addToShaderUniformName(std::string stringExtension) const;
	void detectCurrentShaderProgram();
	void useInterruptedShaderProgram();

	static size_t directionalLightCount;
	size_t directionalLightIndex;

	Shader& shader;
	int interruptedShaderProgramId;
	std::string shaderUniformName;

	glm::vec3 direction;
	glm::vec3 ambient, diffuse, specular;
};

