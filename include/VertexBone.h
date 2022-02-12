#pragma once

#include <iostream>
#include <vector>

class VertexBone
{
public:
	VertexBone();
	~VertexBone();

	void addBone(uint32_t boneId, float weight);
	uint32_t getBoneCount() const;

	uint32_t getBoneId(uint32_t key) const;
	float getBoneWeight(uint32_t key) const;
private:
	std::vector<uint32_t> ids;
	std::vector<float> weights;
};
