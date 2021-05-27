#include "Texture.h"

#include "Debug.h"

Texture::Texture() :
	m_textureId(0u), m_width(0), m_height(0), m_numberOfChannels(0), m_textureType("")
{
	glGenTextures(1, &m_textureId);
}

Texture::Texture(std::filesystem::path texturePath, std::string textureType) :
	m_textureId(0u), m_width(0), m_height(0), m_numberOfChannels(0), m_textureType(textureType)
{
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	stbi_set_flip_vertically_on_load(true);
	unsigned char* textureData = stbi_load(texturePath.string().c_str(), &m_width, &m_height, &m_numberOfChannels, 0);
	if (textureData)
	{
		GLenum textureFormat = GL_INVALID_ENUM;
		if (m_numberOfChannels == 1)
			textureFormat = GL_RED;
		else if (m_numberOfChannels == 3)
			textureFormat = GL_RGB;
		else if (m_numberOfChannels == 4)
			textureFormat = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, m_width, m_height, 0, textureFormat, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glDeleteTextures(1, &m_textureId);
		std::cout << "ERROR: Program could not load texture from: resources/texture.jpg \n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	unbind();
}

Texture::Texture(const Texture& copy)
{
	this->m_width = copy.m_width;
	this->m_height = copy.m_height;
	this->m_numberOfChannels = copy.m_numberOfChannels;
	this->m_textureId = copy.m_textureId;
	this->m_textureType = copy.m_textureType;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureId);
}

void Texture::loadTextureFromDisk(std::filesystem::path texturePath, std::string textureType)
{
	m_textureType = textureType;
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	stbi_set_flip_vertically_on_load(true);
	unsigned char* textureData = stbi_load(texturePath.string().c_str(), &m_width, &m_height, &m_numberOfChannels, 0);
	if (textureData)
	{
		GLenum textureFormat = GL_INVALID_ENUM;
		if (m_numberOfChannels == 1)
			textureFormat = GL_RED;
		else if (m_numberOfChannels == 3)
			textureFormat = GL_RGB;
		else if (m_numberOfChannels == 4)
			textureFormat = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, m_width, m_height, 0, textureFormat, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glDeleteTextures(1, &m_textureId);
		std::cout << "ERROR: Program could not load texture from: resources/texture.jpg \n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	unbind();
}

void Texture::setTextureType(std::string textureType)
{
	m_textureType = textureType;
}

void Texture::bind(unsigned int textureUnitSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureUnitSlot);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0u);
}

unsigned int Texture::getTextureId() const
{
	return m_textureId;
}

std::string Texture::getTextureType() const
{
	return m_textureType;
}
