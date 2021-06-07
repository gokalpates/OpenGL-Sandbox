#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "PointLight.h"

float deltaTime = 0.f, currentFrame, lastFrame = 0.f;

int main()
{
    int screenWidth = 2560;
    int screenHeight = 1440;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Window", NULL, NULL);
    if (window == NULL)
    {
        printf("ERROR: GLFW could not create a window.\n");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("ERROR: Failed to initialise GLAD.\n");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    Camera camera(window);
    camera.setCameraSpeed(15.f);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    //Set this to false or true based on model texturing.
    //For backpack model this is true.
    stbi_set_flip_vertically_on_load(true);

    Shader modelShader("shaders/modelLoading.vert", "shaders/modelLoading.frag");
    Model backpack("resources/models/backpack/backpack.obj");

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.f);
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        camera.update();
        glm::mat4 view = camera.getViewMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            glDepthFunc(GL_GREATER);
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            glDepthFunc(GL_LESS);
        }

        // DRAW OBJECTS
        modelShader.use();
        glm::mat4 model = glm::mat4(1.f);

        modelShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);

        float multiplier = 10.f;
        for (size_t i = 0; i < 10u; i++)
        {
            model = glm::mat4(1.f);
            model = glm::translate(model, glm::vec3(0.f, 0.f, i * multiplier));
            modelShader.setMat4("model", model);

            backpack.draw(modelShader);
        }

        // DRAW GRIDS
        gridShader.use();
        model = glm::mat4(1.f);

        gridShader.setMat4("model", model);
        gridShader.setMat4("view", view);
        gridShader.setMat4("projection", projection);

        grid.draw(gridShader);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}