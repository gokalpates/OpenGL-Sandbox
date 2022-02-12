#include "BoneInspector.h"

#define INSPECT_ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals |  aiProcess_JoinIdenticalVertices )

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

		totalVertex += numOfVertices;
		totalIndex += numOfIndices;
		totalBones += numOfBones;

		if (mesh->HasBones())
		{
			parseBones(mesh);
		}
	}

	std::cout << "\nSum:\n";
	std::cout << "model has " << scene->mNumMeshes << " meshes." << std::endl;
	std::cout << "model has " << totalVertex << " vertices." << std::endl;
	std::cout << "model has " << totalIndex << " indices." << std::endl;
	std::cout << "model has " << totalBones << " bones." << std::endl;
}

void parseBones(const aiMesh* mesh)
{
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		const aiBone* bone = mesh->mBones[i];
		singleBone(bone);
	}
}

void singleBone(const aiBone* bone)
{
	std::cout << bone->mName.C_Str() << " affects " << bone->mNumWeights << " vertex." << std::endl;
}
