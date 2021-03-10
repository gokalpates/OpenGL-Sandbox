#pragma once

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

class Texture
{
public:
	Texture(std::filesystem::path texturePath);
	~Texture();

	void bind(unsigned int textureUnitSlot = 0) const;
	void unbind() const;

	unsigned int getTextureId() const;
private:
	unsigned int m_textureId;
	int m_width, m_height, m_numberOfChannels;
	unsigned int m_textureUnit;
};

