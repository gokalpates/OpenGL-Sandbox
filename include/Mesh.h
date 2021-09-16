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

	unsigned int getVAO() const;
	unsigned int getVBO() const;
	unsigned int getEBO() const;
private:
	void setupBuffers();

	unsigned int VAO, VBO, EBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
};

