#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath);
	~Shader();

	void use() const;
	
	unsigned int getShaderId() const;

	void setBool(const char* name, bool value) const;
	void setFloat(const char* name, float value) const;
	void setInt(const char* name, int value) const;
	void setMat4(const char* name, glm::mat4& matrices) const;
	void setVec3(const char* name, glm::vec3& vector) const;
private:
	unsigned int m_shaderId;
};

