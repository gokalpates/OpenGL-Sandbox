#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	Vertex() 
	{
		position.x = 0.f;
		position.y = 0.f;
		position.z = 0.f;

		normal.x = 0.f;
		normal.y = 0.f;
		normal.z = 0.f;

		texCoordinate.x = 0.f;
		texCoordinate.y = 0.f;
	}

	Vertex(float posX, float posY, float posZ, float norX, float norY, float norZ, float texCorX, float texCorY)
	{
		position.x = posX;
		position.y = posY;
		position.z = posZ;

		normal.x = norX;
		normal.y = norY;
		normal.z = norZ;

		texCoordinate.x = texCorX;
		texCoordinate.y = texCorY;
	}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoordinate;
};
