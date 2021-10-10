#include "FramebufferObject.h"

#include "Debug.h"

FramebufferObject::FramebufferObject(GLFWwindow* window, int fbWidth, int fbHeight):
	m_framebufferId(0u),
	m_textureColorBuffer(0u),
	m_renderbufferId(0u),
	m_fbWidth(fbWidth),
	m_fbHeight(fbHeight),
	m_screenWidth(0),
	m_screenHeight(0)
{
	glfwGetWindowSize(window, &m_screenWidth, &m_screenHeight);

	glGenFramebuffers(1, &m_framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);

	glGenTextures(1, &m_textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer, 0);

	glGenRenderbuffers(1, &m_renderbufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderbufferId);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbWidth, fbHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR: Framebuffer is not complete.\n");
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FramebufferObject::~FramebufferObject()
{
	glDeleteRenderbuffers(1, &m_renderbufferId);
	glDeleteTextures(1, &m_textureColorBuffer);
	glDeleteFramebuffers(1, &m_framebufferId);
}

void FramebufferObject::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
	glViewport(0, 0, m_fbWidth, m_fbHeight);
}

void FramebufferObject::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_screenWidth, m_screenHeight);
}

unsigned int FramebufferObject::getFramebufferObjectId() const
{
	return m_framebufferId;
}

unsigned int FramebufferObject::getTextureId() const
{
	return m_textureColorBuffer;
}

unsigned int FramebufferObject::getRenderbufferId() const
{
	return m_renderbufferId;
}
