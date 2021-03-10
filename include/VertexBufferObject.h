#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

class VertexBufferObject
{
public:
	VertexBufferObject(std::vector<float>& vertices);
	~VertexBufferObject();

	void bind() const;
	void unbind() const;

	unsigned int getVertexBufferObjectId() const;
private:
	unsigned int m_vertexBufferObjectId;
};

