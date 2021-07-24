#include <iostream>
#include <ctime>
#include <cstdlib>

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
    srand(time(0));

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

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    Model terrain("resources/models/basicLand/basicLand.obj");

    Shader vegetableShader("shaders/vegetable.vert", "shaders/vegetable.frag");

    std::vector<Vertex> Quad = {
        Vertex(0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f), //Bottom left
        Vertex(1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f), //Bottom right
        Vertex(0.f,1.f,0.f,0.f,0.f,0.f,0.f,1.f), //Top left

        Vertex(1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f), //Top left
        Vertex(0.f,1.f,0.f,0.f,0.f,0.f,0.f,1.f), //Bottom right
        Vertex(1.f,1.f,0.f,0.f,0.f,0.f,1.f,1.f) //Top right
    };

    GLuint vegetableVAO;
    glGenVertexArrays(1, &vegetableVAO);
    glBindVertexArray(vegetableVAO);

    GLuint vegetableVBO;
    glGenBuffers(1, &vegetableVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vegetableVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Quad.size(), Quad.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,texCoordinate));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint vegetableTexture;
    vegetableTexture = loadTextureFromDisk("resources/grass.png");
    glBindTexture(GL_TEXTURE_2D, 0);

    //BAKING SCENE
#define VEGETABLE_COUNT 30
#define VEGETABLE_DENSITY 5

    std::vector<glm::mat4> vegetableModelMatrices;
    for (size_t i = 0; i < VEGETABLE_COUNT; i++)
    {
        for (size_t j = 0; j < VEGETABLE_COUNT; j++)
        {
            glm::mat4 model(1.f);
            model = glm::translate(model, glm::vec3(j / VEGETABLE_DENSITY, 0, i / VEGETABLE_DENSITY));
            model = glm::rotate(model, glm::radians((float)rand()), glm::vec3(0.f, 1.f, 0.f));
            float scaleX = (rand() % 100) / 16;
            float scaleY = (rand() % 100) / 16;
            float scaleZ = (rand() % 100) / 16;
            model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));
            vegetableModelMatrices.push_back(model);
        }
    }

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
        
        // DRAW VEGETABLE
        vegetableShader.use();
        model = glm::mat4(1.f);

        vegetableShader.setMat4("model", model);
        vegetableShader.setMat4("view", view);
        vegetableShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, vegetableTexture);
        vegetableShader.setInt("texture_diffuse", 0);

        glBindVertexArray(vegetableVAO);
        for (size_t i = 0; i < vegetableModelMatrices.size(); i++)
        {
            vegetableShader.setMat4("model", vegetableModelMatrices.at(i));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, 0);

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