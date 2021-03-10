#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() :
	m_vaoId(0u)
{
	glGenVertexArrays(1, &m_vaoId);
}

VertexArrayObject::~VertexArrayObject()
{
	glDeleteVertexArrays(1, &m_vaoId);
}

void VertexArrayObject::bind() const
{
	glBindVertexArray(m_vaoId);
}

void VertexArrayObject::unbind() const
{
	glBindVertexArray(0u);
}

unsigned int VertexArrayObject::getVertexArrayObjectId() const
{
	return m_vaoId;
}
