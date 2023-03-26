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
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)(screenWidth) / (float)(screenHeight), 0.1f, 10000.f);

    //Camera.
    Camera camera(window);
    camera.setCameraSpeed(65.f);    

    //Grid system.
    Grid grid;
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");

    //General States.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    //Terrain shader.
    Shader terrainTesselationShader("shaders/terrain.vert",
        "shaders/terrain.tesc",
        "shaders/terrain.tese",
        "shaders/terrain.frag");

    //Height map.
    int width = 0, height = 0, nrc = 0;
    unsigned char* data = stbi_load("resources/iceland_heightmap.png", &width, &height, &nrc, 0);
    
    unsigned int heightmap = 0;
    glGenTextures(1, &heightmap);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    //Tesselation patches.
    std::vector<float> vertices;
    unsigned patchCount = 20;
    for (unsigned i = 0; i <= patchCount - 1; i++)
    {
        for (unsigned j = 0; j <= patchCount - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)patchCount); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)patchCount); // v.z
            vertices.push_back(i / (float)patchCount); // u
            vertices.push_back(j / (float)patchCount); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)patchCount); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)patchCount); // v.z
            vertices.push_back((i + 1) / (float)patchCount); // u
            vertices.push_back(j / (float)patchCount); // v

            vertices.push_back(-width / 2.0f + width * i / (float)patchCount); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)patchCount); // v.z
            vertices.push_back(i / (float)patchCount); // u
            vertices.push_back((j + 1) / (float)patchCount); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)patchCount); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)patchCount); // v.z
            vertices.push_back((i + 1) / (float)patchCount); // u
            vertices.push_back((j + 1) / (float)patchCount); // v
        }
    }

    unsigned int terrainVAO = 0;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    unsigned int terrainVBO = 0;
    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0u);
    glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    
    glEnableVertexAttribArray(1u);
    glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0u);

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
        //------------------RENDERPASS START-----------------------------
        model = glm::mat4(1.f);
        terrainTesselationShader.use();
        terrainTesselationShader.setAllMat4(model, view, projection);
        terrainTesselationShader.setInt("heightmap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightmap);
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, 4 * patchCount * patchCount);

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