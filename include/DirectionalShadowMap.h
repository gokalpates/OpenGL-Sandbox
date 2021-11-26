#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class DirectionalShadowMap
{
public:
	DirectionalShadowMap(GLFWwindow* window);
	~DirectionalShadowMap();

	void bind();
	void unbind();

	void openDebugMode();
	void closeDebugMode();

	bool isUpdateRequired() const;
	void update();

	void setShadowPosition(glm::vec3 position);
	void setShadowLookAtPosition(glm::vec3 position);
	void setShadowMapSize(float size);
	void setNear(float near);
	void setFar(float far);

	unsigned int getFramebufferId() const;
	unsigned int getDepthTextureId() const;
	unsigned int getShadowMapResolution() const;
	glm::vec3 getShadowPosition() const;
	glm::vec3 getShadowLookAtPosition() const;
	float getShadowMapSize() const;
	float getNear() const;
	float getFar() const;
	glm::mat4& getLightSpaceMatrix();

private:
	glm::mat4 createNewOrthogonalMatrix();
	glm::mat4 createNewViewMatrix();
	void calculateLightSpaceMatrix();

	unsigned int m_framebufferId, m_depthTextureId, m_shadowMapResolution;
	int m_screenWidth, m_screenHeight;
	glm::vec3 m_shadowPosition, m_shadowLookAtPosition;
	glm::mat4 m_shadowMapOrthogonal, m_shadowMapView, m_lightSpaceMatrix;
	float m_shadowMapSize, m_near, m_far;
	bool m_isViewUpdateRequired, m_isOrthoUpdateRequired;
};

