#include "SkinnedModel.h"

SkinnedModel::SkinnedModel() :
	m_VAO(0u),
	m_Buffers {0},
	m_Scene(nullptr)
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

	m_Scene = m_Importer.ReadFile(filePath, ASSIMP_LOAD_FLAGS);

	if (m_Scene)
	{
		m_GlobalInverseTransform = utils::mat4_cast(m_Scene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
		extractInfo(m_Scene);
		processScene(m_Scene);
	}
	else
	{
		isSuccess = false;
		std::cout << "ERROR: Assimp could not load model: " << m_Importer.GetErrorString() << std::endl;
	}

	loadToVideoMemory();
	clearFromMainMemory();

	return isSuccess;
}

void SkinnedModel::draw(Shader& shader)
{
	glBindVertexArray(m_VAO);

	for (size_t i = 0; i < m_MeshInfos.size(); i++)
	{
		uint32_t diffuseCount = 0u;
		uint32_t specularCount = 0u;

		MeshMaterial& material = m_Materials.at(i);
		for (size_t j = 0; j < material.m_Textures.size(); j++)
		{
			glActiveTexture(GL_TEXTURE0 + j);

			std::string index;
			std::string name = material.m_Textures.at(j).m_TextureType;
			if (name == "diffuse")
			{
				index = std::to_string(diffuseCount);
				diffuseCount++;
			}
			else if (name == "specular")
			{
				index = std::to_string(specularCount);
				specularCount++;
			}

			glUniform1i(glGetUniformLocation(shader.m_shaderId, ("material." + name + index).c_str()), j);
			glBindTexture(GL_TEXTURE_2D, material.m_Textures.at(j).m_TextureId);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, m_MeshInfos[i].m_NumOfIndex, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * m_MeshInfos[i].m_BaseIndex), m_MeshInfos[i].m_BaseVertex);
		//glDrawElements(GL_TRIANGLES, m_MeshInfos.at(i).m_NumOfIndex, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * m_MeshInfos.at(i).m_BaseIndex));
	}
}

uint32_t SkinnedModel::getUniqueBoneCount() const
{
	return m_BoneMap.size();
}

void SkinnedModel::getBoneTransforms(std::vector<glm::mat4>& transforms, long double timeInSeconds)
{
	transforms.resize(m_BoneInfos.size());
	glm::mat4 identity(1.f);

	float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0);
	float timeInTicks = timeInSeconds * ticksPerSecond;
	float animationTimeTicks = std::fmod(timeInTicks, (float)m_Scene->mAnimations[0]->mDuration);

	processNodeHierarchy(m_Scene->mRootNode, identity, animationTimeTicks);

	for (size_t i = 0; i < m_BoneInfos.size(); i++)
	{
		transforms[i] = m_BoneInfos[i].finalTransform;
	}
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
		tempMeshInfo.m_BaseIndex = totalIndexCount;
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
			temp2D.y = textureCoords.y;
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

	if (boneID == m_BoneInfos.size())
	{
		BoneInfo info(utils::mat4_cast(bone->mOffsetMatrix));
		m_BoneInfos.push_back(info);
	}

	for (size_t i = 0; i < bone->mNumWeights; i++)
	{
		const aiVertexWeight& vw = bone->mWeights[i];
		uint32_t globalVertexId = m_MeshInfos[meshId].m_BaseVertex + vw.mVertexId;
		m_BonesPerVertex.at(globalVertexId).addBoneData(boneID, vw.mWeight);
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

void SkinnedModel::processNodeHierarchy(const aiNode* node, const glm::mat4 parentTransform, float animationTimeTicks)
{
	std::string nodeName(node->mName.C_Str());
	glm::mat4 nodeTransformation(utils::mat4_cast(node->mTransformation));

	//There can be different animations.
	const aiAnimation* animation = m_Scene->mAnimations[0];
	const aiNodeAnim* nodeAnim = findNodeAnim(animation, nodeName);

	if (nodeAnim)
	{
		aiVector3D scaling;
		calculateInterpolatedScaling(scaling, animationTimeTicks, nodeAnim);
		glm::vec3 scalingGLM = glm::vec3(scaling.x, scaling.y, scaling.z);
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.f), scalingGLM);
		
		aiQuaternion rotation;
		calculateInterpolatedRotation(rotation, animationTimeTicks, nodeAnim);
		glm::quat rotationGLM = utils::quat_cast(rotation);
		glm::mat4 rotationMatrix = glm::mat4_cast(rotationGLM);

		aiVector3D translation;
		calculateInterpolatedTranslation(translation, animationTimeTicks, nodeAnim);
		glm::vec3 translationGLM = glm::vec3(translation.x, translation.y, translation.z);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), translationGLM);

		nodeTransformation = translationMatrix * rotationMatrix * scalingMatrix;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransformation;

	if (m_BoneMap.contains(nodeName))
	{
		uint32_t boneIndex = m_BoneMap[nodeName];
		m_BoneInfos[boneIndex].finalTransform = m_GlobalInverseTransform * globalTransformation * m_BoneInfos.at(boneIndex).offsetMatrix;
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		processNodeHierarchy(node->mChildren[i], globalTransformation, animationTimeTicks);
	}
}

aiNodeAnim* SkinnedModel::findNodeAnim(const aiAnimation* animation, std::string nodeName)
{
	for (size_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* node = animation->mChannels[i];
		std::string nodeAnimName(node->mNodeName.C_Str());
		if (nodeAnimName == nodeName)
		{
			return node;
		}
	}
	return nullptr;
}

void SkinnedModel::calculateInterpolatedScaling(aiVector3D& scaling, float animationTimeTicks, const aiNodeAnim* node)
{
	if (node->mNumScalingKeys == 1)
	{
		scaling = node->mScalingKeys[0].mValue;
		return;
	}

	uint32_t ScalingIndex = findScaling(animationTimeTicks, node);
	uint32_t NextScalingIndex = ScalingIndex + 1;
	float t1 = (float)node->mScalingKeys[ScalingIndex].mTime;
	float t2 = (float)node->mScalingKeys[NextScalingIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (animationTimeTicks - (float)t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = node->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = node->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	scaling = Start + Factor * Delta;
}

void SkinnedModel::calculateInterpolatedRotation(aiQuaternion& rotation, float animationTimeTicks, const aiNodeAnim* node)
{
	if (node->mNumRotationKeys == 1) {
		rotation = node->mRotationKeys[0].mValue;
		return;
	}

	uint32_t RotationIndex = findRotation(animationTimeTicks, node);
	uint32_t NextRotationIndex = RotationIndex + 1;

	float t1 = (float)node->mRotationKeys[RotationIndex].mTime;
	float t2 = (float)node->mRotationKeys[NextRotationIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (animationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = node->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = node->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(rotation, StartRotationQ, EndRotationQ, Factor);
	rotation = StartRotationQ;
	rotation.Normalize();
}

void SkinnedModel::calculateInterpolatedTranslation(aiVector3D& translation, float animationTimeTicks, const aiNodeAnim* node)
{
	if (node->mNumPositionKeys == 1) {
		translation = node->mPositionKeys[0].mValue;
		return;
	}

	uint32_t PositionIndex = findTranslation(animationTimeTicks, node);
	uint32_t NextPositionIndex = PositionIndex + 1;

	float t1 = (float)node->mPositionKeys[PositionIndex].mTime;
	float t2 = (float)node->mPositionKeys[NextPositionIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (animationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = node->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = node->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	translation = Start + Factor * Delta;
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

uint32_t SkinnedModel::findScaling(float animationTimeTicks, const aiNodeAnim* node)
{
	for (uint32_t i = 0; i < node->mNumScalingKeys - 1; i++) {
		float t = (float)node->mScalingKeys[i + 1].mTime;
		if (animationTimeTicks < t)
		{
			return i;
		}
	}

	return 0;
}

uint32_t SkinnedModel::findRotation(float animationTimeTicks, const aiNodeAnim* node)
{
	for (uint32_t i = 0; i < node->mNumRotationKeys - 1; i++) {
		float t = (float)node->mRotationKeys[i + 1].mTime;
		if (animationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

uint32_t SkinnedModel::findTranslation(float animationTimeTicks, const aiNodeAnim* node)
{
	for (uint32_t i = 0; i < node->mNumPositionKeys - 1; i++) {
		float t = (float)node->mPositionKeys[i + 1].mTime;
		if (animationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}
