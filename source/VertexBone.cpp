#include "VertexBone.h"

VertexBone::VertexBone()
{
}

VertexBone::~VertexBone()
{
	ids.clear();
	weights.clear();
}

void VertexBone::addBone(uint32_t boneId, float weight)
{
	ids.push_back(boneId);
	weights.push_back(weight);
}

uint32_t VertexBone::getBoneCount() const
{
	return weights.size(); //Or ids.size();
}

uint32_t VertexBone::getBoneId(uint32_t key) const
{
	return ids.at(key);
}

float VertexBone::getBoneWeight(uint32_t key) const
{
	return weights.at(key);
}
