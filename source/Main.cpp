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
int fpsToShow = 0;
size_t counter = 0;

int main()
{
    const int screenWidth = 2560, screenHeight = 1440;

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

    inspectModel("resources/objects/boblamp/bob_lamp_update.md5mesh");

    Shader heat("shaders/skeletalHeatMap.vert", "shaders/skeletalHeatMap.frag");
    SkinnedModel skinnedModel;
    skinnedModel.loadSkinnedModel("resources/objects/boblamp/bob_lamp_update.md5mesh");
    
    uint32_t spacebarCount = 0u;
    double timer = 0;

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

        if (glfwGetKey(window, GLFW_KEY_SPACE) && (timer > 1))
        {
            timer = 0;
            spacebarCount++;
            heat.use();
            heat.setInt("displayBoneIndex", spacebarCount % skinnedModel.getUniqueBoneCount());
        }
        timer += deltaTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        heat.use();
        model = glm::mat4(1.f);
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        heat.setAllMat4(model, view, projection);
        skinnedModel.draw(heat);

        gridShader.use();
        model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);
        grid.draw(gridShader);

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;
    }

    releaseResources();

    glfwDestroyWindow(window);
    glfwTerminate();
}