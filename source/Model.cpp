#include "Model.h"

Model::Model(std::string path, unsigned int count, glm::mat4* matricesArray):
	instanceCount(count),
	instancedArray(matricesArray)
{
	loadModel(path);
}

void Model::draw(Shader& shader)
{
	for (size_t i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("ERROR: Assimp could not load model: %s \n", importer.GetErrorString());
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene, scene->mRootNode);
}

void Model::processNode(const aiScene* scene, aiNode* node)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(scene, mesh));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(scene, node->mChildren[i]);
	}
}

Mesh Model::processMesh(const aiScene* scene, aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vertex.position = position;

		if (mesh->HasNormals())
		{
			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			vertex.normal = normal;
		}
		else
		{
			vertex.normal = glm::vec3(0.f, 0.f, 0.f);
		}

		if (mesh->HasTextureCoords(0))
		{
			glm::vec2 textureCoordinates;
			textureCoordinates.x = mesh->mTextureCoords[0][i].x;
			textureCoordinates.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoordinate = textureCoordinates;
		}
		else
		{
			vertex.texCoordinate = glm::vec2(0.f, 0.f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	if (instanceCount > 1)
	{
		return Mesh(vertices, indices, textures, instanceCount, instancedArray);
	}
	else
	{
		return Mesh(vertices, indices, textures);
	}
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString string;
		material->GetTexture(type, i, &string);

		bool isTextureLoadedAlready = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path.data(), string.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				isTextureLoadedAlready = true;
				break;
			}
		}
		if (!isTextureLoadedAlready)
		{   
			Texture texture;
			texture.id = loadTextureFromDisk(string.C_Str(), directory);
			texture.type = typeName;
			texture.path = string.C_Str();
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::loadTextureFromDisk(const char* path, std::string directory)
{
	std::string fileName = std::string(path);
	fileName = directory + '/' + fileName;

	GLuint textureId;
	glGenTextures(1, &textureId);

	int width, height, numberOfChannels;

	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &numberOfChannels, 0);
	if (data)
	{
		GLenum textureFormat = GL_INVALID_ENUM;
		if (numberOfChannels == 1)
		{
			textureFormat = GL_RED;
		}
		else if (numberOfChannels == 3)
		{
			textureFormat = GL_RGB;
		}
		else if (numberOfChannels == 4)
		{
			textureFormat = GL_RGBA;
		}
		else
		{
			printf("ERROR: Invalid texture format. \n");
			glfwTerminate();
			std::exit(EXIT_FAILURE);
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		printf("ERROR: Texture could not load from: %s ", path);
		stbi_image_free(data);
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	return textureId;
}
