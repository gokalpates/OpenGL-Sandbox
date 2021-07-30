#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
size_t counter = 0;

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
    camera.setCameraSpeed(10.f);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    Model cathedral("resources/models/cathedral/combined02.obj");

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.f);
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        currentTime = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        diffTime = currentTime - lastTime;
        lastFrame = currentFrame;
        
        if (diffTime >= 1.0)
        {
            std::string FPS = "FPS: " + std::to_string(counter);
            glfwSetWindowTitle(window, FPS.c_str());
            counter = 0;
            lastTime = currentTime;
        }

        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        camera.update();
        glm::mat4 view = camera.getViewMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //If 'E' key is pressed then enable back face culling.
        if (glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        //If 'R' key is pressed then disable back face culling.
        if (glfwGetKey(window,GLFW_KEY_R) == GLFW_PRESS)
        {
            glDisable(GL_CULL_FACE);
        }

        // DRAW CATHEDRAL
        modelShader.use();
        glm::mat4 model = glm::mat4(1.f);

        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

        modelShader.setMat4("model", model);
        modelShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);

        cathedral.draw(modelShader);

        // DRAW GRIDS
        gridShader.use();
        model = glm::mat4(1.f);

        gridShader.setMat4("model", model);
        gridShader.setMat4("view", view);
        gridShader.setMat4("projection", projection);

        grid.draw(gridShader);
  
        //------------------SWAP BUFFERS------------------
        glfwSwapBuffers(window);
        counter++;
    }

    glfwTerminate();
}