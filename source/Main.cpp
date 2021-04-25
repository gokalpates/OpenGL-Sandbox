#include <iostream>
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

	std::vector<float> vertices = {
	// Vertex Positions in local space.
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
	};

	VertexArrayObject lightSourceVAO;
	lightSourceVAO.bind();

	VertexBufferObject lightSourceVBO(vertices);
	lightSourceVBO.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	lightSourceVBO.unbind();
	lightSourceVAO.unbind();

	VertexArrayObject illuminatedVAO;
	illuminatedVAO.bind();

	VertexBufferObject illuminatedVBO(vertices);
	illuminatedVBO.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	illuminatedVBO.unbind();
	illuminatedVAO.unbind();

	Shader lightSourceShader("shaders/vertex.vert", "shaders/lightSource.frag");
	Shader illuminatedObjectShader("shaders/vertex.vert", "shaders/illuminated.frag");

	Camera camera(window);

	glm::vec3 objectColor(0.31f, 0.69f, 0.42f);
	glm::vec3 lightColor(0.2f, 0.4f, 0.9f);
	glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

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

		glm::mat4 model(1.f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2));

		glm::mat4 view(1.f);
		view = camera.getViewMatrix();

		glm::mat4 projection(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeigth, 0.1f, 100.f);

		lightSourceShader.use();
		lightSourceShader.setMat4("model", model);
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setVec3("lightColor", lightColor);

		lightSourceVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Draw illuminated object.
		
		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.f));

		illuminatedObjectShader.use();
		illuminatedObjectShader.setMat4("model", model);
		illuminatedObjectShader.setMat4("view", view);
		illuminatedObjectShader.setMat4("projection", projection);
		illuminatedObjectShader.setVec3("lightColor", lightColor);
		illuminatedObjectShader.setVec3("objectColor", objectColor);

		illuminatedVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
	}
}