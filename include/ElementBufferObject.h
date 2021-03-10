#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

class ElementBufferObject
{
public:
	ElementBufferObject(std::vector<unsigned int>& indices);
	~ElementBufferObject();

	void bind() const;
	void unbind() const;

	unsigned int getElementBufferObjectId() const;
	int getIndicesCount() const;

private:
	unsigned int m_elementBufferObjectId;
	int m_indicesCount;
};

