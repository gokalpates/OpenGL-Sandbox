#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"
#include "Utilities.h"
#include "BoneInspector.h"
#include "SkinnedModel.h"

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
long double applicationStartTime = 0.0, applicationCurrentTime = 0.0, applicationElapsedTime;
int fpsToShow = 0;
size_t counter = 0;

int main()
{
    const int screenWidth = 2560, screenHeight = 1440;

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
    
    //MVP Matrices and view position vector.
    glm::mat4 model = glm::mat4(1.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::vec3 viewPosition = glm::vec3(0.f);
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)(screenWidth) / (float)(screenHeight), 0.1f, 100.f);

    //Camera.
    Camera camera(window);
    
    //Grid system.
    Grid grid;
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");

    //General States.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    //Compute shader example.
    Shader computeShader("shaders/compute.comp");

    unsigned int storageTexture = 0u;
    glGenTextures(1, &storageTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, storageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

    Shader quadShader("shaders/quad.vert", "shaders/quad.frag");

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

        camera.update();
        view = camera.getViewMatrix();
        viewPosition = camera.getCameraPosition();

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Use compute shader.
        double cursorX, cursorY;
        glfwGetCursorPos(window, &cursorX, &cursorY);
        bool mouseClick = false;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            mouseClick = true;
        }

        glm::vec3 mousePosition(cursorX, screenHeight - cursorY, 0.f);

        computeShader.use();
        computeShader.setVec3("mousePos", mousePosition);
        computeShader.setBool("mouseButtonClick", mouseClick);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, storageTexture);
        glBindImageTexture(1, storageTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glDispatchCompute(80,45,1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //Use texture that generated in compute shaders.
        quadShader.use();
        quadShader.setInt("tex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, storageTexture);
        utils::renderQuad();

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;

        //------------------TIME UNTIL APP STARTS------------------
        applicationCurrentTime = glfwGetTime() * 1000.f;
        applicationElapsedTime = applicationCurrentTime - applicationStartTime;
    }

    glDeleteTextures(1, &storageTexture);

    glfwDestroyWindow(window);
    glfwTerminate();
}