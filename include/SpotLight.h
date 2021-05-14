#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

class SpotLight
{
public:
	SpotLight(Shader& shader);
	~SpotLight();

	void updateAllUniforms();

	void setPosition(glm::vec3 position);
	void setDirection(glm::vec3 direction);
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);
	void setLinear(float linear);
	void setQuadratic(float quadratic);
	void setInnerCutoff(float innerCutoff);
	void setOuterCutoff(float outerCutoff);

	std::string getShaderUniformName() const;
	size_t getLightIndex() const;
	static size_t getLightCount();
	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	glm::vec3 getAmbient() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;
	float getLinear() const;
	float getQuadratic() const;
	float getInnerCutoff() const;
	float getOuterCutoff() const;

private:
	void setShader(Shader& shader);
	void incrementSpotLightCount() const;
	void reduceSpotLightCount() const;
	void createShaderUniformName();
	std::string addToShaderUniformName(std::string stringExtension) const;
	void detectCurrentShaderProgram();
	void useInterruptedShaderProgram();

	static size_t spotLightCount;
	size_t spotLightIndex;

	Shader& shader;
	int interruptedShaderProgramId;
	std::string shaderUniformName;

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient, diffuse, specular;

	float linear, quadratic;
	float innerCutoff, outerCutoff;
};

