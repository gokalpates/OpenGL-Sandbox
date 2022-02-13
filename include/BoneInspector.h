#pragma once

#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexBone.h"

void inspectModel(std::string model);
void parseScene(const aiScene* scene);
void parseMeshes(const aiScene* scene);
void parseBones(uint32_t meshId, const aiMesh* mesh);
void singleBone(uint32_t meshId, const aiBone* bone);

uint32_t getBoneId(const aiBone* bone);
uint32_t getUniqueBoneSize();
void printMappedData();

//Release background resources needed to parse model.
void releaseResources();
