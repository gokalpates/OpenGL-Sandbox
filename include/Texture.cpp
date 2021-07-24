#include "Texture.h"

GLuint loadTextureFromDisk(const char* path)
{
	GLuint textureId;
	glGenTextures(1, &textureId);

	int width, height, numberOfChannels;
	unsigned char* data = stbi_load(path, &width, &height, &numberOfChannels, 0);
	if (data)
	{
		GLenum textureFormat = GL_INVALID_ENUM;
		if (numberOfChannels == 1)
		{
			textureFormat = GL_RED;
		}
		else if (numberOfChannels == 3)
		{
			textureFormat = GL_RGB;
		}
		else if (numberOfChannels == 4)
		{
			textureFormat = GL_RGBA;
		}
		else
		{
			printf("ERROR: Invalid texture format. \n");
			glfwTerminate();
			std::exit(EXIT_FAILURE);
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		printf("ERROR: Texture could not load from: %s ", path);
		stbi_image_free(data);
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	return textureId;
}