#include <iostream>
#include <thread>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Callback.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "ElementBufferObject.h"
#include "VertexBufferObject.h"
#include "Camera.h"
#include "Debug.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

float deltaTime = 0.f, currentFrame, lastFrame = 0.f;

int main()
{
	int windowWidth = 2560, windowHeigth = 1440;

	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeigth, "OpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "ERROR: Program could not create a window: \n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window, windowSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR: GLAD could not initialised: \n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_DEPTH_TEST);

	//GRIDS
	std::vector<float> axisVertices = {
	 100.f,  0.f,  0.f, //X
	-100.f,  0.f,  0.f,

	 0.f,   100.f, 0.f, //Y
	 0.f,  -100.f, 0.f,

	 0.f,   0.f, 100.f, //Z
	 0.f,   0.f,-100.f
	};

	Shader axisShader("shaders/axis.vert", "shaders/axis.frag");

	VertexArrayObject axisVAO;
	axisVAO.bind();

	VertexBufferObject axisVBO(axisVertices);
	axisVBO.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	axisVBO.unbind();
	axisVAO.unbind();

	std::vector<float> cubeVertices = {
	//Positions			  //Normals			   //Texture Coords.
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	//LIGHT SOURCE
	Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");

	VertexArrayObject lightSourceVAO;
	lightSourceVAO.bind();

	VertexBufferObject cubeVBO(cubeVertices);
	cubeVBO.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	cubeVBO.unbind();
	lightSourceVAO.unbind();

	//CUBE
	Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");

	VertexArrayObject objectVAO;
	objectVAO.bind();

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);

	Texture objectDiffuseMap;
	objectDiffuseMap.loadTextureFromDisk("resources/diffuseMap.png", "diffuseMap");
	objectDiffuseMap.bind(0);

	lightingShader.setInt("material.specular", 1);

	Texture objectSpecularMap;
	objectSpecularMap.loadTextureFromDisk("resources/specularMap.png", "specularMap");
	objectSpecularMap.bind(1);

	cubeVBO.bind();
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	cubeVBO.unbind();
	objectDiffuseMap.unbind();
	objectSpecularMap.unbind();
	objectVAO.unbind();

	Camera camera(window);

	//Numbers of the lights must match with numbers of the lights in fragment shader.
	DirectionalLight directionalLight(lightingShader);
	directionalLight.setDirection(glm::vec3(0.f, 0.f, 1.f));

	PointLight pointLightOne(lightingShader);
	pointLightOne.setPosition(glm::vec3(3.f, 0.f, 0.f));
	pointLightOne.setAmbient(glm::vec3(0.05f, 0.f, 0.f));
	pointLightOne.setDiffuse(glm::vec3(0.7f, 0.f, 0.f));
	pointLightOne.setSpecular(glm::vec3(1.f, 0.f, 0.f));

	PointLight pointLightTwo(lightingShader);
	pointLightTwo.setPosition(glm::vec3(-3.f, 0.f, 0.f));
	pointLightTwo.setAmbient(glm::vec3(0.f, 0.05f, 0.f));
	pointLightTwo.setDiffuse(glm::vec3(0.f, 0.7f, 0.f));
	pointLightTwo.setSpecular(glm::vec3(0.f, 1.f, 0.f));

	SpotLight spotLight(lightingShader);
	spotLight.setPosition(glm::vec3(0.f, 0.f, 3.f));
	spotLight.setDirection(glm::vec3(0.f, 0.f, -1.f));
	spotLight.setAmbient(glm::vec3(0.f, 0.f, 0.05f));
	spotLight.setDiffuse(glm::vec3(0.f, 0.f, 0.7f));
	spotLight.setSpecular(glm::vec3(0.f, 0.f, 1.f));

	lightingShader.setFloat("material.shininess", 32.f);
	glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeigth, 0.1f, 100.f);
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		camera.update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.f);
		glm::mat4 view = camera.getViewMatrix();

		//DRAW CUBE
		lightingShader.use();
		objectVAO.bind();
		objectDiffuseMap.bind(0);
		objectSpecularMap.bind(1);

		model = glm::mat4(1.f);

		lightingShader.setMat4("model", model);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);

		glm::vec3 viewPosition = camera.getCameraPosition();
		lightingShader.setVec3("viewPosition", viewPosition);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//DRAW GRIDS
		axisShader.use();
		axisVAO.bind();

		model = glm::mat4(1.f);

		axisShader.setMat4("model", model);
		axisShader.setMat4("view", view);
		axisShader.setMat4("projection", projection);

		axisShader.setVec3("color", 1.f, 0.f, 0.f);
		glDrawArrays(GL_LINES, 0, 2);

		axisShader.setVec3("color", 0.f, 1.f, 0.f);
		glDrawArrays(GL_LINES, 2, 2);

		axisShader.setVec3("color", 0.f, 0.f, 1.f);
		glDrawArrays(GL_LINES, 4, 2);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}