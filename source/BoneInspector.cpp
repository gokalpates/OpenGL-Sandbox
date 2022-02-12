#include "BoneInspector.h"

#define INSPECT_ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals |  aiProcess_JoinIdenticalVertices )

std::map<std::string, uint32_t> uniqueBones;
std::vector<VertexBone> vertexBoneData;
std::vector<uint32_t> baseVertex;

void inspectModel(std::string model)
{
	std::cout << "Inspecting model named as: " << model << "\n\n";

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model, INSPECT_ASSIMP_LOAD_FLAGS);
	if (!scene)
	{
		std::cout << "ERROR: Could not inspect model because Assimp could not read from: " << model << std::endl;
		return;
	}

	parseScene(scene);
}

void parseScene(const aiScene* scene)
{
	parseMeshes(scene);
}

void parseMeshes(const aiScene* scene)
{
	unsigned int totalVertex = 0;
	unsigned int totalIndex = 0;
	unsigned int totalBones = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		
		unsigned int numOfVertices = mesh->mNumVertices;
		unsigned int numOfIndices = mesh->mNumFaces * 3;
		unsigned int numOfBones = mesh->mNumBones;

		baseVertex.push_back(totalVertex);

		totalVertex += numOfVertices;
		totalIndex += numOfIndices;
		totalBones += numOfBones;

		vertexBoneData.resize(totalVertex);

		if (mesh->HasBones())
		{
			parseBones(i, mesh);
		}
	}

	std::cout << "\nSum:\n";
	std::cout << "model has " << scene->mNumMeshes << " meshes." << std::endl;
	std::cout << "model has " << totalVertex << " vertices." << std::endl;
	std::cout << "model has " << totalIndex << " indices." << std::endl;
	std::cout << "model has " << totalBones << " bones." << std::endl;
}

void parseBones(uint32_t meshId, const aiMesh* mesh)
{
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		const aiBone* bone = mesh->mBones[i];
		singleBone(meshId, bone);
	}
}

void singleBone(uint32_t meshId, const aiBone* bone)
{
	uint32_t id = getBoneId(bone);

	std::cout << bone->mName.C_Str() << " (" << id << ") " << bone->mNumWeights << " vertex." << std::endl;

	for (size_t i = 0; i < bone->mNumWeights; i++)
	{
		const aiVertexWeight& vw = bone->mWeights[i];

		uint32_t globalVertexId = baseVertex[meshId] + vw.mVertexId;
		vertexBoneData.at(globalVertexId).addBone(id, vw.mWeight);
	}
}

//Maps bone and returns its id.
uint32_t getBoneId(const aiBone* bone)
{
	if (uniqueBones.contains(bone->mName.C_Str()))
	{
		return uniqueBones[bone->mName.C_Str()];
	}
	else
	{
		uint32_t boneId = uniqueBones.size();
		uniqueBones[bone->mName.C_Str()] = boneId;

		return boneId;
	}
}

uint32_t getUniqueBoneSize()
{
	return uniqueBones.size();
}

void printMappedData()
{
	for (size_t i = 0; i < vertexBoneData.size(); i++)
	{
		const VertexBone& vb = vertexBoneData.at(i);
		for (size_t j = 0; j < vb.getBoneCount(); j++)
		{
			uint32_t boneId = vb.getBoneId(j);
			float boneWeight = vb.getBoneWeight(j);

			std::cout << "Vertex(" << i << ") has affected by bone(" << boneId << ") with weight(" << boneWeight << ").\n";
		}
	}
}

void releaseResources()
{
	std::cout << "Releasing inspector resources.\n";

	baseVertex.clear();
	vertexBoneData.clear();
	uniqueBones.clear();
}
