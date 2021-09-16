#pragma once

#include <vector>
#include <string>

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, unsigned int instanceCount = 1u, glm::mat4* instancedArray = nullptr);

	void draw(Shader& shader);
private:
	void setupBuffers();

	unsigned int VAO, VBO, EBO;

	unsigned int instanceCount;
	unsigned int instanceVBO;
	glm::mat4* instancedArray;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
};

