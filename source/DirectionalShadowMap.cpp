#include "DirectionalShadowMap.h"

DirectionalShadowMap::DirectionalShadowMap(GLFWwindow* window) :
	m_framebufferId(0u),
	m_depthTextureId(0u),
	m_shadowMapResolution(1024u),
	m_screenWidth(0),
	m_screenHeight(0),
	m_shadowPosition(glm::vec3(100.f, 100.f, 100.f)),
	m_shadowLookAtPosition(glm::vec3(0.f, 0.f, 0.f)),
	m_shadowMapOrthogonal(glm::mat4(1.f)),
	m_shadowMapView(glm::mat4(1.f)),
	m_lightSpaceMatrix(glm::mat4(1.f)),
	m_shadowMapSize(15.f),
	m_near(0.1f),
	m_far(1000.f),
	m_isViewUpdateRequired(false),
	m_isOrthoUpdateRequired(false)
{
	glfwGetWindowSize(window, &m_screenWidth, &m_screenHeight);

	glGenFramebuffers(1, &m_framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);

	glGenTextures(1, &m_depthTextureId);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureId);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowMapResolution, m_shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.f,1.f,1.f,1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureId, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_shadowMapOrthogonal = createNewOrthogonalMatrix();
	m_shadowMapView = createNewViewMatrix();
	m_lightSpaceMatrix = m_shadowMapOrthogonal * m_shadowMapView;
}

DirectionalShadowMap::~DirectionalShadowMap()
{
	glDeleteTextures(1, &m_depthTextureId);
	glDeleteFramebuffers(1, &m_framebufferId);
}

void DirectionalShadowMap::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
	glViewport(0, 0, m_shadowMapResolution, m_shadowMapResolution);
}

void DirectionalShadowMap::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_screenWidth, m_screenHeight);
}

void DirectionalShadowMap::openDebugMode()
{
	glBindTexture(GL_TEXTURE_2D, m_depthTextureId);

	float borderColor[] = { 0.f,0.f,0.f,0.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectionalShadowMap::closeDebugMode()
{
	glBindTexture(GL_TEXTURE_2D, m_depthTextureId);

	float borderColor[] = { 1.f,1.f,1.f,1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool DirectionalShadowMap::isUpdateRequired() const
{
	if (m_isOrthoUpdateRequired || m_isViewUpdateRequired)
		return true;
	return false;
}

void DirectionalShadowMap::update()
{
	if (m_isViewUpdateRequired)
	{
		m_isViewUpdateRequired = false;
		m_shadowMapView = createNewViewMatrix();
	}
	if (m_isOrthoUpdateRequired)
	{
		m_isOrthoUpdateRequired = false;
		m_shadowMapOrthogonal = createNewOrthogonalMatrix();
	}
	calculateLightSpaceMatrix();
}

void DirectionalShadowMap::setShadowPosition(glm::vec3 position)
{
	m_isViewUpdateRequired = true;
	m_shadowPosition = position;
}

void DirectionalShadowMap::setShadowLookAtPosition(glm::vec3 position)
{
	m_isViewUpdateRequired = true;
	m_shadowLookAtPosition = position;
}

void DirectionalShadowMap::setShadowMapSize(float size)
{
	m_isOrthoUpdateRequired = true;
	m_shadowMapSize = size;
}

void DirectionalShadowMap::setNear(float near)
{
	m_isOrthoUpdateRequired = true;
	m_near = near;
}

void DirectionalShadowMap::setFar(float far)
{
	m_isOrthoUpdateRequired = true;
	m_far = far;
}

unsigned int DirectionalShadowMap::getFramebufferId() const
{
	return m_framebufferId;
}

unsigned int DirectionalShadowMap::getDepthTextureId() const
{
	return m_depthTextureId;
}

unsigned int DirectionalShadowMap::getShadowMapResolution() const
{
	return m_shadowMapResolution;
}

glm::vec3 DirectionalShadowMap::getShadowPosition() const
{
	return m_shadowPosition;
}

glm::vec3 DirectionalShadowMap::getShadowLookAtPosition() const
{
	return m_shadowLookAtPosition;
}

float DirectionalShadowMap::getShadowMapSize() const
{
	return m_shadowMapSize;
}

float DirectionalShadowMap::getNear() const
{
	return m_near;
}

float DirectionalShadowMap::getFar() const
{
	return m_far;
}

glm::mat4& DirectionalShadowMap::getLightSpaceMatrix()
{
	return m_lightSpaceMatrix;
}

glm::mat4 DirectionalShadowMap::createNewOrthogonalMatrix()
{
	glm::mat4 matrix(1.f);
	matrix = glm::ortho(-m_shadowMapSize, m_shadowMapSize, -m_shadowMapSize, m_shadowMapSize, m_near, m_far);
	return matrix;
}

glm::mat4 DirectionalShadowMap::createNewViewMatrix()
{
	glm::mat4 matrix(1.f);
	matrix = glm::lookAt(m_shadowPosition, m_shadowLookAtPosition, glm::vec3(0.f, 1.f, 0.f));
	return matrix;
}

void DirectionalShadowMap::calculateLightSpaceMatrix()
{
	m_lightSpaceMatrix = m_shadowMapOrthogonal * m_shadowMapView;
}
