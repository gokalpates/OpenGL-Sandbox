#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "Mesh.h"
#include "Shader.h"

unsigned int loadTextureFromDisk(const char* path, std::string directory);

class Model
{
public:
	Model(std::string path);

	void draw(Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> loadedTextures;

	void loadModel(std::string path);

	void processNode(const aiScene* scene, aiNode* node);
	Mesh processMesh(const aiScene* scene, aiMesh* mesh);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};

