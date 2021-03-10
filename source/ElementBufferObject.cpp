#include "ElementBufferObject.h"

ElementBufferObject::ElementBufferObject(std::vector<unsigned int>& indices) :
    m_elementBufferObjectId(0u), m_indicesCount(0)
{
    m_indicesCount = indices.size();

    glGenBuffers(1, &m_elementBufferObjectId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObjectId);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ElementBufferObject::~ElementBufferObject()
{
    glDeleteBuffers(1, &m_elementBufferObjectId);
}

void ElementBufferObject::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObjectId);
}

void ElementBufferObject::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int ElementBufferObject::getElementBufferObjectId() const
{
    return m_elementBufferObjectId;
}

int ElementBufferObject::getIndicesCount() const
{
    return m_indicesCount;
}
