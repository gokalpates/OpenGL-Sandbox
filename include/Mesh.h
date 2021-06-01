#pragma once

#include <vector>
#include <string>

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void draw(Shader& shader);
private:
	void setupBuffers();

	unsigned int VAO, VBO, EBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
};

