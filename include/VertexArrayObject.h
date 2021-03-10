#pragma once

#include <glad/glad.h>

class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();

	void bind() const;
	void unbind() const;

	unsigned int getVertexArrayObjectId() const;
private:
	unsigned int m_vaoId;
};

