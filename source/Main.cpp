#include <iostream>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "PointLight.h"
#include "Debug.h"

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
    camera.setCameraSpeed(10.f);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    Model terrain("resources/models/basicLand/basicLand.obj");

    Shader blendingShader("shaders/blending.vert", "shaders/blending.frag");
    std::vector<Vertex> Quad = {
        Vertex(0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f), //Bottom left
        Vertex(1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f), //Bottom right
        Vertex(0.f,1.f,0.f,0.f,0.f,0.f,0.f,1.f), //Top left

        Vertex(1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f), //Top left
        Vertex(0.f,1.f,0.f,0.f,0.f,0.f,0.f,1.f), //Bottom right
        Vertex(1.f,1.f,0.f,0.f,0.f,0.f,1.f,1.f) //Top right
    };

    GLuint windowVAO;
    glGenVertexArrays(1, &windowVAO);
    glBindVertexArray(windowVAO);

    GLuint windowVBO;
    glGenBuffers(1, &windowVBO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Quad.size(), Quad.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoordinate));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint windowTexture;
    windowTexture = loadTextureFromDisk("resources/redTransparentWindow.png");

    std::vector<glm::vec3> windowPositions;
    windowPositions.push_back(glm::vec3(0.f, 1.f, 1.f));
    windowPositions.push_back(glm::vec3(1.f, 2.f, 2.f));
    windowPositions.push_back(glm::vec3(2.f, 3.f, 3.f));
    windowPositions.push_back(glm::vec3(3.f, 4.f, 4.f));
    std::map<float, glm::vec3> windowOrderedPositions;

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

        // DRAW TERRAIN
        modelShader.use();
        glm::mat4 model = glm::mat4(1.f);

        modelShader.setMat4("model", model);
        modelShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);

        terrain.draw(modelShader);

        // DRAW GRIDS
        gridShader.use();
        model = glm::mat4(1.f);

        gridShader.setMat4("model", model);
        gridShader.setMat4("view", view);
        gridShader.setMat4("projection", projection);

        grid.draw(gridShader);

        // DRAW WINDOWS
       
        // Order windows.
        windowOrderedPositions.clear();
        for (size_t i = 0; i < windowPositions.size(); i++)
        {
            float distance = glm::length(camera.getCameraPosition() - windowPositions.at(i));
            windowOrderedPositions[distance] = windowPositions.at(i);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        blendingShader.use();

        blendingShader.setMat4("view", view);
        blendingShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        blendingShader.setInt("texture_diffuse", 0);

        glBindVertexArray(windowVAO);
        for (std::map<float,glm::vec3>::reverse_iterator it = windowOrderedPositions.rbegin(); it != windowOrderedPositions.rend(); it++)
        {
            model = glm::mat4(1.f);
            model = glm::translate(model, it->second);
            model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));
            blendingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, Quad.size());
        }
        glDisable(GL_BLEND);
        
        //------------------SWAP BUFFERS------------------
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}