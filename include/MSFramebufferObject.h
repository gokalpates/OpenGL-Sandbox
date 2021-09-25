#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Multisampling framebuffer.
class MSFramebufferObject
{
public:
	MSFramebufferObject(GLFWwindow* window, unsigned int samples = 1);
	~MSFramebufferObject();

	void bind();
	void unbind();

	unsigned int getFramebufferObjectId() const;
	unsigned int getSamplingCount() const;
private:
	unsigned int m_sampleCount;

	unsigned int m_framebufferId;
	unsigned int m_textureColorBuffer;
	unsigned int m_renderbufferId;

	int m_screenWidth, m_screenHeight;
};

