#pragma once

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

class Texture
{
public:
	explicit Texture();
	Texture(std::filesystem::path texturePath, std::string textureType);
	Texture(const Texture& copy);
	~Texture();

	void loadTextureFromDisk(std::filesystem::path texturePath, std::string textureType);

	void setTextureType(std::string textureType);
	
	void bind(unsigned int textureUnitSlot = 0) const;
	void unbind() const;

	unsigned int getTextureId() const;
	std::string getTextureType() const;

private:
	unsigned int m_textureId;
	int m_width, m_height, m_numberOfChannels;
	std::string m_textureType;
};

