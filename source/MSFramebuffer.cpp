#include "MSFramebuffer.h"

MSFramebufferObject::MSFramebufferObject(GLFWwindow* window, unsigned int samples):
	m_sampleCount(samples),
	m_framebufferId(0u),
	m_textureColorBuffer(0u),
	m_renderbufferId(0u),
	m_screenWidth(0),
	m_screenHeight(0)
{
	glfwGetWindowSize(window, &m_screenWidth, &m_screenHeight);
	
	int maximumSampleCount = 0;
	glGetIntegerv(GL_MAX_SAMPLES, &maximumSampleCount);

	if (m_sampleCount > maximumSampleCount)
		m_sampleCount = maximumSampleCount;
	
	glGenFramebuffers(1, &m_framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);

	glGenTextures(1, &m_textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureColorBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_RGB, m_screenWidth, m_screenHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureColorBuffer, 0);

	glGenRenderbuffers(1, &m_renderbufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderbufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sampleCount, GL_DEPTH24_STENCIL8, m_screenWidth, m_screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR: Multisampling framebuffer could not created.\n");
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MSFramebufferObject::~MSFramebufferObject()
{
	glDeleteRenderbuffers(1, &m_renderbufferId);
	glDeleteTextures(1, &m_textureColorBuffer);
	glDeleteFramebuffers(1, &m_framebufferId);
}

void MSFramebufferObject::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
	//Framebuffer has same width and height with screen.
	//So, we dont need to set viewport.
}

void MSFramebufferObject::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int MSFramebufferObject::getFramebufferObjectId() const
{
	return m_framebufferId;
}

unsigned int MSFramebufferObject::getSamplingCount() const
{
	return m_sampleCount;
}
