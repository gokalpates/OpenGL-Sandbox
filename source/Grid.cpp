#include "Grid.h"

Grid::Grid()
{
	setupBuffers();
}

void Grid::draw(Shader& shader)
{
	glBindVertexArray(VAO);

	glUniform3f(glGetUniformLocation(shader.m_shaderId, "color"), 1.f, 0.f, 0.f);
	glDrawArrays(GL_LINES, 0, 2);

	glUniform3f(glGetUniformLocation(shader.m_shaderId, "color"), 0.f, 1.f, 0.f);
	glDrawArrays(GL_LINES, 2, 2);

	glUniform3f(glGetUniformLocation(shader.m_shaderId, "color"), 0.f, 0.f, 1.f);
	glDrawArrays(GL_LINES, 4, 2);
}

void Grid::setupBuffers()
{
	std::vector<Vertex> vertices = {
		// X axis.
		Vertex(-100.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f),
		Vertex(100.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f),
		// Y axis.
		Vertex(0.f,-100.f,0.f,0.f,0.f,0.f,0.f,0.f),
		Vertex(0.f,100.f,0.f,0.f,0.f,0.f,0.f,0.f),
		// Z axis.
		Vertex(0.f,0.f,-100.f,0.f,0.f,0.f,0.f,0.f),
		Vertex(0.f,0.f,100.f,0.f,0.f,0.f,0.f,0.f)
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));

	glBindVertexArray(0);
}
