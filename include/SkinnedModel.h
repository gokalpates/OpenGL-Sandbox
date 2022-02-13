#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>     
#include <assimp/postprocess.h>

#include "Shader.h"

#define ASSIMP_LOAD_FLAGS aiProcess_Triangulate | aiProcess_FlipUVs

class SkinnedModel
{
public:
	SkinnedModel();
	~SkinnedModel();

	bool loadModel(std::string path);

private:
	void loadToVideoMemory();
	void clear();
	void clearFromMainMemory();
	void clearFromVideoMemory();

	void extractInfo(const aiScene* scene);

	void processScene(const aiScene* scene);
	void processMeshes(const aiScene* scene);
	void processSingleMesh(const uint32_t meshId, const aiMesh* mesh);
	void processMeshBones(const uint32_t meshId, const aiMesh* mesh);
	void processSingleBone(const uint32_t meshId, const aiBone* bone);

	uint32_t getBoneId(const aiBone* bone);

	enum BUFFER_TYPE
	{
		INDEX_BUFFER = 0,
		POSITION_BUFFER,
		TEXCOORD_BUFFER,
		NORMAL_BUFFER,
		BONE_BUFFER,
		NUM_BUFFERS
	};
 
	enum LOCATION
	{
		POSITION_LOC    = 0,
		TEXCOORD_LOC    = 1,
		NORMAL_LOC      = 2,
		BONE_ID_LOC     = 3,
		BONE_WEIGHT_LOC = 4
	};

#define MAX_BONE_PER_VERTEX 4u

	struct VertexBone
	{
		uint32_t m_BoneIds[MAX_BONE_PER_VERTEX] = { 0u };
		float m_Weights[MAX_BONE_PER_VERTEX] = { 0.f };

		void addBoneData(uint32_t boneId, float weight)
		{
			for (uint32_t i = 0; i < MAX_BONE_PER_VERTEX; i++)
			{
				if (m_Weights[i] == 0.f)
				{
					m_BoneIds[i] = boneId;
					m_Weights[i] = weight;
					return;
				}
			}

			// Bone count per vertex is limited to MAX_BONE_PER_VERTEX macro.
			// Application tried to load more bone than it allowed. Try to increase MAX_BONE_PER_VERTEX.
			assert(1);
		}
	};

	struct  MeshInfo
	{
		uint32_t m_MeshId = 0u;
		std::string m_MeshName = "";
		uint32_t m_NumOfIndex = 0u;
		uint32_t m_NumOfVertex = 0u;
		uint32_t m_BaseVertex = 0u;
	};

	struct ModelInfo
	{
		uint32_t m_MeshCount = 0u;
		uint32_t m_TotalNumOfVertex = 0u;
		uint32_t m_TotalNumOfIndex = 0u;
	};

	ModelInfo info;

	GLuint m_VAO;
	GLuint m_Buffers[NUM_BUFFERS];

	std::vector<uint32_t> m_Indices;
	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<glm::vec3> m_Normals;
	std::vector<VertexBone> m_BonesPerVertex;

	std::map<std::string, uint32_t> m_BoneMap;
	std::vector<MeshInfo> m_MeshInfos;
};

