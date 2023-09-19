#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Debug.h"
#include "Utilities.h"
#include "Grid.h"

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
long double applicationStartTime = 0.0, applicationCurrentTime = 0.0, applicationElapsedTime;
int fpsToShow = 0;
size_t counter = 0;

int main()
{
    const int screenWidth = 1920, screenHeight = 1080;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

    //General States.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera camera(window);
    Shader gridShader("shaders/infiniteGrid.vert",
                              "shaders/infiniteGrid.frag");

    unsigned int vao = 0u;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Note that it is in milliseconds.
    applicationStartTime = glfwGetTime() * 1000.f;
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        currentTime = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        diffTime = currentTime - lastTime;
        lastFrame = currentFrame;
        
        if (diffTime >= 1.0)
        {
            fpsToShow = counter;
            counter = 0;
            lastTime = currentTime;
            glfwSetWindowTitle(window, std::to_string(fpsToShow).c_str());
        }

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);
        
        camera.update();

        glm::vec3 viewPosition = camera.getCameraPosition();

        glm::mat4 model = glm::mat4(1.f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //------------------RENDERPASS START-----------------------------
        gridShader.use();
        gridShader.setMat4("view", view);
        gridShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        std::cout << viewPosition.x << " || " << viewPosition.y << " || " << viewPosition.z << "\n";

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;

        //------------------TIME UNTIL APP STARTS------------------
        applicationCurrentTime = glfwGetTime() * 1000.f;
        applicationElapsedTime = applicationCurrentTime - applicationStartTime;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}