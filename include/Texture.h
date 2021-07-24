#pragma once

#include <string>

#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

GLuint loadTextureFromDisk(const char* path);