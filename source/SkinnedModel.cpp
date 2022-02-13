#include "SkinnedModel.h"

SkinnedModel::SkinnedModel()
{
}

SkinnedModel::~SkinnedModel()
{
	clear();
}

bool SkinnedModel::loadSkinnedModel(std::string filePath)
{
	clear();

	info.m_DirPath = calculateDirPath(filePath);
	bool isSuccess = true;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, ASSIMP_LOAD_FLAGS);

	if (scene)
	{
		extractInfo(scene);
		processScene(scene);
	}
	else
	{
		isSuccess = false;
		std::cout << "ERROR: Assimp could not load model: " << importer.GetErrorString() << std::endl;
	}

	loadToVideoMemory();
	clearFromMainMemory();

	return isSuccess;
}

void SkinnedModel::loadToVideoMemory()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(NUM_BUFFERS, m_Buffers);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), m_Indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITION_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_Positions.size() * sizeof(glm::vec3), m_Positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOC);
	glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_TexCoords.size() * sizeof(glm::vec2), m_TexCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEXCOORD_LOC);
	glVertexAttribPointer(TEXCOORD_LOC, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(glm::vec3), m_Normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOC);
	glVertexAttribPointer(NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_BonesPerVertex.size() * sizeof(VertexBone), m_BonesPerVertex.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOC);
	glVertexAttribIPointer(BONE_ID_LOC, MAX_BONE_PER_VERTEX, GL_UNSIGNED_INT, sizeof(VertexBone), 0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOC);
	glVertexAttribPointer(BONE_WEIGHT_LOC, MAX_BONE_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBone), (void*)(MAX_BONE_PER_VERTEX * sizeof(uint32_t)));

	glBindVertexArray(0);
}

void SkinnedModel::clear()
{
	clearFromMainMemory();
	clearFromVideoMemory();
}

void SkinnedModel::clearFromMainMemory()
{
	if (!m_Indices.empty())
		m_Indices.clear();

	if (!m_Positions.empty())
		m_Positions.clear();

	if (!m_TexCoords.empty())
		m_TexCoords.clear();

	if (!m_Normals.empty())
		m_Normals.clear();

	if (!m_BonesPerVertex.empty())
		m_BonesPerVertex.clear();
}

void SkinnedModel::clearFromVideoMemory()
{
	if (m_Buffers[POSITION_BUFFER] != 0)
		glDeleteBuffers(NUM_BUFFERS, m_Buffers);

	if (m_VAO != 0u)
		glDeleteVertexArrays(1, &m_VAO);
}

void SkinnedModel::extractInfo(const aiScene* scene)
{
	info.m_MeshCount = scene->mNumMeshes;

	uint32_t totalVertexCount = 0u;
	uint32_t totalIndexCount = 0u;

	MeshInfo tempMeshInfo;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		tempMeshInfo.m_MeshId = i;
		tempMeshInfo.m_MeshName = mesh->mName.C_Str();
		tempMeshInfo.m_BaseVertex = totalVertexCount;
		tempMeshInfo.m_NumOfVertex = mesh->mNumVertices;
		tempMeshInfo.m_NumOfIndex = mesh->mNumFaces * 3;

		totalVertexCount += tempMeshInfo.m_NumOfVertex;
		totalIndexCount += tempMeshInfo.m_NumOfIndex;

		m_MeshInfos.push_back(tempMeshInfo);
	}

	info.m_TotalNumOfIndex = totalIndexCount;
	info.m_TotalNumOfVertex = totalVertexCount;

	m_BonesPerVertex.resize(totalVertexCount);
}

std::string SkinnedModel::calculateDirPath(std::string path)
{
	return path.substr(0, path.find_last_of('/'));
}

void SkinnedModel::processScene(const aiScene* scene)
{
	//Process meshes, camera, lights, etc.
	processMeshes(scene);
}

void SkinnedModel::processMeshes(const aiScene* scene)
{
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		processSingleMesh(i, mesh);

		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		processMeshMaterial(material);
	}
}

void SkinnedModel::processSingleMesh(const uint32_t meshId, const aiMesh* mesh)
{
	glm::vec3 temp3D;
	glm::vec2 temp2D;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		const aiVector3D& position = mesh->mVertices[i];
		temp3D.x = position.x;
		temp3D.y = position.y;
		temp3D.z = position.z;
		m_Positions.push_back(temp3D);

		if (mesh->HasNormals())
		{
			const aiVector3D& normal = mesh->mNormals[i];
			temp3D.x = normal.x;
			temp3D.y = normal.y;
			temp3D.z = normal.z;
			m_Normals.push_back(temp3D);
		}
		else
		{
			temp3D.x = 0.f;
			temp3D.y = 0.f;
			temp3D.z = 0.f;
			m_Normals.push_back(temp3D);
		}

		if (mesh->HasTextureCoords(0))
		{
			const aiVector3D textureCoords = mesh->mTextureCoords[0][i];
			temp2D.x = textureCoords.x;
			temp2D.y = textureCoords.z;
			m_TexCoords.push_back(temp2D);
		}
		else
		{
			temp2D.x = 0.f;
			temp2D.y = 0.f;
			m_TexCoords.push_back(temp2D);
		}
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		m_Indices.push_back(face.mIndices[0]);
		m_Indices.push_back(face.mIndices[1]);
		m_Indices.push_back(face.mIndices[2]);
	}

	processMeshBones(meshId, mesh);
}

void SkinnedModel::processMeshBones(const uint32_t meshId, const aiMesh* mesh)
{
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		const aiBone* bone = mesh->mBones[i];
		processSingleBone(meshId, bone);
	}
}

void SkinnedModel::processSingleBone(const uint32_t meshId, const aiBone* bone)
{
	uint32_t boneID = getBoneId(bone);

	for (size_t i = 0; i < bone->mNumWeights; i++)
	{
		const aiVertexWeight& vw = bone->mWeights[i];
		uint32_t globalVertexId = m_MeshInfos[meshId].m_BaseVertex + vw.mVertexId;
		m_BonesPerVertex.at(i).addBoneData(boneID, vw.mWeight);
	}
}

void SkinnedModel::processMeshMaterial(const aiMaterial* material)
{
	MeshMaterial tempMeshMaterial;
	
	std::vector<Texture> diffuseTextures = processTextureType(material, aiTextureType_DIFFUSE, "diffuse");
	tempMeshMaterial.m_Textures.insert(tempMeshMaterial.m_Textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	std::vector<Texture> specularTextures = processTextureType(material, aiTextureType_SPECULAR, "specular");
	tempMeshMaterial.m_Textures.insert(tempMeshMaterial.m_Textures.end(), specularTextures.begin(), specularTextures.end());

	m_Materials.push_back(tempMeshMaterial);
}

std::vector<SkinnedModel::Texture> SkinnedModel::processTextureType(const aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	Texture tempTexture;

	for (size_t i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString textureName;
		material->GetTexture(type, i, &textureName);

		//Check for textures if they already allocates space in vram.

		bool isAlreadyLoaded = false;
		for (size_t j = 0; j < m_LoadedTextures.size(); j++)
		{
			std::string tempString = textureName.C_Str();
			if (m_LoadedTextures.at(j).m_TexturePath == (info.m_DirPath + "/" + tempString))
			{
				isAlreadyLoaded = true;
				textures.push_back(m_LoadedTextures[j]);
				break;
			}
		}

		if (!isAlreadyLoaded)
		{
			tempTexture.m_TexturePath = info.m_DirPath + "/" + std::string(textureName.C_Str());
			tempTexture.m_TextureType = typeName;
			tempTexture.m_TextureId = loadTextureFromDisk(tempTexture.m_TexturePath);
			textures.push_back(tempTexture);

			m_LoadedTextures.push_back(tempTexture);
		}
	}

	return textures;
}

uint32_t SkinnedModel::loadTextureFromDisk(std::string path)
{
	uint32_t textureId = 0u;
	glGenTextures(1, &textureId);

	int width, height, numberOfChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfChannels, 0);
	if (data)
	{
		GLenum textureformat = GL_INVALID_ENUM;
		if (numberOfChannels == 1)
			textureformat = GL_RED;
		else if (numberOfChannels == 3)
			textureformat = GL_RGB;
		else if (numberOfChannels == 4)
			textureformat = GL_RGBA;
		else
			std::cout << "ERROR: Invalid texture format. \n";

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, textureformat, width, height, 0, textureformat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "ERROR: Texture could not load from: " << path << "\n";
		stbi_image_free(data);
	}

	return textureId;
}

uint32_t SkinnedModel::getBoneId(const aiBone* bone)
{
	std::string boneName = bone->mName.C_Str();
	if (m_BoneMap.contains(boneName))
	{
		return m_BoneMap[boneName];
	}
	else
	{
		m_BoneMap[boneName] = m_BoneMap.size();
		return m_BoneMap[boneName];
	}
}
