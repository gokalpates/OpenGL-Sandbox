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
    camera.setCameraSpeed(3.f);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    //Set this to false or true based on model texturing.
    //For backpack model this is true.
    stbi_set_flip_vertically_on_load(true);

    Shader modelShader("shaders/modelLoading.vert", "shaders/modelLoading.frag");
    Shader modelLightingShader("shaders/modelLoadingLighting.vert", "shaders/modelLoadingLighting.frag");
    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");

    Model backpack("resources/models/backpack/backpack.obj");
    Model sphere("resources/models/sphere/sphere.obj");

    modelLightingShader.use();
    PointLight pointLightOne(modelLightingShader);
    pointLightOne.setPosition(glm::vec3(3.f, 0.f, 0.f));
    pointLightOne.setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
    pointLightOne.setDiffuse(glm::vec3(0.7f, 0.7f, 0.7f));
    pointLightOne.setSpecular(glm::vec3(1.f, 1.f, 1.f));

    modelLightingShader.setFloat("material.shininess", 32.f);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 300.f);
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

        modelLightingShader.use();
        glm::vec3 cameraPosition = camera.getCameraPosition();
        modelLightingShader.setVec3("viewPosition", cameraPosition);

        if (glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (glfwGetKey(window,GLFW_KEY_R) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //DRAW MODEL.

        modelLightingShader.use();
        glm::mat4 model = glm::mat4(1.f);

        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        modelLightingShader.setMat4("model", model);
        modelLightingShader.setMat4("view", view);
        modelLightingShader.setMat4("projection", projection);

        backpack.draw(modelLightingShader);

        //Draw Grid.

        gridShader.use();
        model = glm::mat4(1.f);

        gridShader.setMat4("model", model);
        gridShader.setMat4("view", view);
        gridShader.setMat4("projection", projection);

        grid.draw(gridShader);

        //DRAW LIGHT SOURCE

        lightSourceShader.use();
        model = glm::mat4(1.f);

        float x = std::cos(glfwGetTime());
        float z = std::sin(glfwGetTime());

        pointLightOne.setPosition(glm::vec3(x,0.f,z));

        model = glm::translate(model, pointLightOne.getPosition());
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);

        glm::vec3 pointLightColor = pointLightOne.getSpecular();
        lightSourceShader.setVec3("light.color", pointLightColor);

        sphere.draw(lightSourceShader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}