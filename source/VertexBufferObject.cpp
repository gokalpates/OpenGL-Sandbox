#include "VertexBufferObject.h"

VertexBufferObject::VertexBufferObject(std::vector<float>& vertices) :
	m_vertexBufferObjectId(0u)
{
	glGenBuffers(1, &m_vertexBufferObjectId);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBufferObject::~VertexBufferObject()
{
	glDeleteBuffers(1, &m_vertexBufferObjectId);
}

void VertexBufferObject::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);
}

void VertexBufferObject::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int VertexBufferObject::getVertexBufferObjectId() const
{
	return m_vertexBufferObjectId;
}
