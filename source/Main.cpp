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

float deltaTime = 0.f, currentFrame, lastFrame = 0.f;

int main()
{
	srand(time(nullptr));

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

	//AXIS
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

	// LIGHT
	Shader lightShader("shaders/light.vert", "shaders/light.frag");

	std::vector<float> cubeVertices = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	VertexArrayObject lightVAO;
	lightVAO.bind();

	VertexBufferObject cubeVBO(cubeVertices);
	cubeVBO.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	cubeVBO.unbind();
	lightVAO.unbind();

	// OBJECT
	Shader objectShader("shaders/object.vert", "shaders/object.frag");

	VertexArrayObject objectVAO;
	objectVAO.bind();

	cubeVBO.bind();
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	cubeVBO.unbind();
	objectVAO.unbind();

	Camera camera(window);

	glm::vec3 objectColor(1.0f, 0.31f, 0.42f);
	glm::vec3 lightColor(1.f, 1.f, 1.f);
	glm::vec3 lightPosition(2.0, 1.f, 1.5f);
	glm::vec3 viewPosition(1.f);

	lightShader.use();
	lightShader.setVec3("light.color", lightColor);

	objectShader.use();
	objectShader.setVec3("light.color", lightColor);

	glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeigth, 0.1f, 100.f);
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.update();
		
		//Draw light source.
		lightShader.use();
		lightVAO.bind();

		glm::mat4 model(1.f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		glm::mat4 view = camera.getViewMatrix();
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Draw illuminated object.
		objectShader.use();
		objectVAO.bind();

		model = glm::mat4(1.f);
		objectShader.setMat4("model", model);
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);

		viewPosition = camera.getCameraPosition();
		objectShader.setVec3("viewPosition", viewPosition);

		objectShader.setVec3("light.position", lightPosition);
		objectShader.setVec3("light.ambient", 1.f, 1.f, 1.f);
		objectShader.setVec3("light.diffuse", 1.f, 1.f, 1.f);
		objectShader.setVec3("light.specular", 1.f, 1.f, 1.f);

		objectShader.setVec3("material.ambient", 0.24725, 0.1995, 0.0745);
		objectShader.setVec3("material.diffuse", 0.75164, 0.60648, 0.22648);
		objectShader.setVec3("material.specular", 0.628281, 0.555802, 0.366065);
		objectShader.setFloat("material.shininess", 128.f);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Draw grids.
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
}