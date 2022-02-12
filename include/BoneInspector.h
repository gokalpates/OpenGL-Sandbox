#pragma once

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>     
#include <assimp/postprocess.h>

void inspectModel(std::string model);
void parseScene(const aiScene* scene);
void parseMeshes(const aiScene* scene);
void parseBones(const aiMesh* mesh);
void singleBone(const aiBone* bone);


