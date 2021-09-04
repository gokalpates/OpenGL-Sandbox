#pragma once

#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "stb_image.h"

#include "Camera.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox(std::vector<std::string>& resources, Camera* camera, glm::mat4* projection);
	~Skybox();

	void draw() const;
private:
	static std::vector<float> cubemapVertices;
	Camera* camera;
	Shader* skyboxShader;
	glm::mat4* projectionMatrix;

	unsigned int cubeVAO, cubeVBO;
	unsigned int cubemapTexture;
	
};

