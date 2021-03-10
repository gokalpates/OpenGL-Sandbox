#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Callback.h" //Includes definition of windowSizeCallback nothing more.
#include "Shader.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "ElementBufferObject.h"
#include "VertexBufferObject.h"

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

	glClearColor(0.4f, 0.f, 0.f, 1.f);

	Shader program("shaders/vertexShader.vert", "shaders/fragmentShader.frag");

	std::vector<float> vertices = {
	-0.5f, -0.5f, -0.5f,  0.f, 0.f, 0.f,   0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.f, 0.f, 1.f,   1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.f, 1.f, 0.f,   1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.f, 1.f, 1.f,   1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.f, 0.f, 0.f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.f, 0.f, 1.f,   0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.f, 0.f, 0.f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.f, 0.f, 1.f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.f, 1.f, 0.f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.f, 1.f, 1.f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.f, 0.f, 1.f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  0.f, 0.f, 0.f,   1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.f, 0.f, 1.f,   1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.f, 1.f, 0.f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.f, 1.f, 1.f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.f, 0.f, 0.f,   0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.f, 0.f, 1.f,   1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  0.f, 0.f, 0.f,   1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.f, 0.f, 1.f,   1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.f, 1.f, 0.f,   0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.f, 1.f, 1.f,   0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.f, 0.f, 0.f,   0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.f, 0.f, 1.f,   1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.f, 0.f, 0.f,   0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.f, 0.f, 1.f,   1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.f, 1.f, 0.f,   1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.f, 1.f, 1.f,   1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.f, 0.f, 0.f,   0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  1.f, 0.f, 1.f,   0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.f, 0.f, 0.f,   0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.f, 0.f, 1.f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.f, 1.f, 0.f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.f, 1.f, 1.f,   1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,   0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.f, 0.f, 1.f,   0.0f, 1.0f
	};

	std::vector<unsigned int> drawIndices = {
		0,1,2, //First triangle
		3,2,0  //Second triangle
	};

	VertexArrayObject testVao;
	testVao.bind();

	VertexBufferObject testVbo(vertices);
	testVbo.bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	ElementBufferObject testEbo(drawIndices);
	testEbo.bind();

	testVbo.unbind();
	testVao.unbind();

	Texture testTexture("resources/texture.jpg");
	testTexture.bind();
	testVao.bind();
	program.use();
	program.setInt("texture0", 0);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//Update start.

		if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glm::mat4 model(1.f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.f, 0.5f, 0.f));

		glm::mat4 view(1.f);
		view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

		glm::mat4 projection(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeigth, 0.1f, 100.f);

		program.setMat4("model", model);
		program.setMat4("view", view);
		program.setMat4("projection", projection);
		//Update end.

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
	}
}