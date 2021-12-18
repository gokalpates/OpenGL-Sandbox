#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, unsigned int count, glm::mat4* matricesArray)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->instanceCount = count;
	this->instancedArray = matricesArray;
	this->instanceVBO = 0u;

	setupBuffers();
}

void Mesh::draw(Shader& shader)
{
	GLuint diffuseCount = 0;
	GLuint specularCount = 0;
	GLuint normalCount = 0;
	for (GLuint i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string index;
		std::string name = textures[i].type;

		if (name == "diffuse")
		{
			index = std::to_string(diffuseCount);
			diffuseCount++;
		}
		else if (name == "specular")
		{
			index = std::to_string(specularCount);
			specularCount++;
		}
		else if (name == "normal")
		{
			index = std::to_string(normalCount);
			normalCount++;
		}

		glUniform1i(glGetUniformLocation(shader.m_shaderId, ("material." + name + index).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	if (instanceCount > 1)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Mesh::setupBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoordinate)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, bitangent)));

	if (instanceCount > 1)
	{
		glGenBuffers(1, &instanceVBO);

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::mat4), instancedArray, GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1* sizeof(glm::vec4)));

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));

		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
	}

	glBindVertexArray(0);
}
