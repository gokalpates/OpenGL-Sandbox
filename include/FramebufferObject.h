#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FramebufferObject
{
public:
	FramebufferObject(GLFWwindow* window, int fbWidth, int fbHeight);
	~FramebufferObject();

	void bind();
	void unbind();

	unsigned int getFramebufferObjectId() const;
	unsigned int getTextureId() const;
	unsigned int getRenderbufferId() const;
private:
	unsigned int m_framebufferId;
	unsigned int m_textureColorBuffer;
	unsigned int m_renderbufferId;

	int m_fbWidth, m_fbHeight;
	int m_screenWidth, m_screenHeight;
};

