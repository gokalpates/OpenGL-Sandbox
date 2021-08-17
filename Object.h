#pragma once

#include "Model.h"
#include "Shader.h"

struct Transform {
	glm::vec3 translate;

	glm::vec3 rotate;
	float rotateAngle;

	glm::vec3 scale;
};

class Object
{
public:
	Object(std::string modelPath, Shader& shaderToDraw, std::string objectName = "null");
	~Object();

	void translate();
	void rotate();
	void scale();

	void draw();
private:
	std::string objectName;
	Model objectModel;
	Shader objectShader;
	Transform objectTransform;
};

