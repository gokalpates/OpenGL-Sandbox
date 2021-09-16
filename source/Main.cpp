#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "FramebufferObject.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"
#include "Skybox.h"

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
int fpsToShow = 0;
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);

    Camera camera(window);
    camera.setCameraSpeed(50.f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.f);

    std::vector<std::string> skyboxLocations = {
        "resources/skybox/right.png",
        "resources/skybox/left.png",
        "resources/skybox/top.png",
        "resources/skybox/bottom.png",
        "resources/skybox/front.png",
        "resources/skybox/back.png" //If output is wrong then swap last 2.
    };
    Skybox skybox(skyboxLocations, &camera, &projection);

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    Shader instanceModelShader("shaders/instancing.vert", "shaders/instancing.frag");
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    unsigned int amount = 100000;
    glm::mat4* modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime());
    float radius = 100.0;
    float offset = 20.f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
    unsigned int index = 0u;
        float angle = (float)i / (float)amount * 360.0f;
        
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        
        model = glm::translate(model, glm::vec3(x, y, z));
        {
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));
    }
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
    glm::vec3 offsets[100];
        modelMatrices[i] = model;
    }
            index++;
    Model planet("resources/models/Planet/planet.obj");
    Model asteroid("resources/models/Asteroid/rock.obj", 100000, modelMatrices);
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        model = glm::scale(model, glm::vec3(4.f, 4.f, 4.f));
        modelShader.setAllMat4(model, view, projection);
        planet.draw(modelShader);
            offsets[index] = glm::vec3(x, y, 0.f);
        instanceModelShader.use();
        instanceModelShader.setMat4("view", view);
        instanceModelShader.setMat4("projection", projection);
        asteroid.draw(instanceModelShader);
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            offsets[index] = glm::vec3(x, y, 0.f);
            index++;
        }
    }

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);

    glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::vec3), &offsets, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

    Shader instanceTest("shaders/instancing.vert", "shaders/instancing.frag");

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
        }

        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window,GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("OpenGL Renderer");
        ImGui::Text("FPS: %d", fpsToShow);
        ImGui::End();

        camera.update();
        view = camera.getViewMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelShader.use();
        glm::mat4 model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);
        grid.draw(gridShader);

        instanceTest.use();
        instanceTest.setAllMat4(model, view, projection);
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        //Be sure to draw skybox last even in all conditions.
        skybox.draw();

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        counter++;

    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}