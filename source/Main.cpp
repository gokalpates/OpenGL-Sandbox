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
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    Camera camera(window);
    camera.setCameraSpeed(15.f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.f);

    std::vector<std::string> skyboxLocations = {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg" //If output is wrong then swap last 2.
    };
    Skybox sky(skyboxLocations, &camera, &projection);
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    Model box("resources/models/Wooden Box/woodenBox.obj");

    //Bake offsets.
    unsigned int index = 0u;
    glm::vec3 offsets[100];
    for (size_t z = 0; z < 20; z=z+2)
    {
        for (size_t x = 0; x < 20; x=x+2)
        {
            offsets[index] = glm::vec3(x, 0.f, z);
            index++;
        }
    }

    //Set offsets in model shader.
    modelShader.use();
    for (size_t i = 0; i < 100; i++)
    {
        std::string queryString = "offsets[" + std::to_string(i) + "]";
        if (i == 50) //Just for debug and demonstration purposes set 50th containers position to 0.f,5.f,0.f.
        {
            glm::vec3 temp = glm::vec3(0.f, 5.f, 0.f);
            modelShader.setVec3(queryString.c_str(), temp);
            continue;
        }
        modelShader.setVec3(queryString.c_str(), offsets[i]);
    }

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
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("model", model);
        box.draw(modelShader); //Be aware that mesh classes draw function is altered for instanced drawing.

        gridShader.use();
        model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);
        grid.draw(gridShader);

        //Be sure to draw sky last even in all conditions.
        sky.draw();

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