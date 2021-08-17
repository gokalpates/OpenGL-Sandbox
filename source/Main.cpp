#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"

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
    bool editorLayer = false;
    glm::mat4 editorMat4 = glm::mat4(1.f);

    glEnable(GL_DEPTH_TEST);

    Camera camera(window);
    camera.setCameraSpeed(10.f);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    Model sandTerrain("resources/models/Sand Terrain/sandTerrain.obj");
    Model woodenBox("resources/models/Wooden Box/woodenBox.obj");
    
    float inputLocation[3] = { 0,0,0 };
    float inputRotation[3] = { 0,0,0 };
    float inputScale[3] = { 1,1,1 };
    float backgroundColor[4] = { 0,0,0,1 };

    glm::mat4 view = camera.getViewMatrix();
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
            fpsToShow = counter;
            counter = 0;
            lastTime = currentTime;
        }

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera.setFirstTouch(true);
            editorMat4 = view;
            editorLayer = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            editorLayer = false;
        }

        if (editorLayer)
        {
            view = editorMat4;
            ImGui::Begin("OpenGL Renderer");
            ImGui::Text("Mode: Editor Mode.");
            ImGui::Text("FPS: %d", fpsToShow);
            ImGui::End();

            ImGui::Begin("Transform");
            ImGui::InputFloat3("Location", inputLocation);
            ImGui::InputFloat3("Rotation", inputRotation);
            ImGui::InputFloat3("Scale", inputScale);
            ImGui::End();

            ImGui::Begin("Sky");
            ImGui::ColorEdit4("Background Color", backgroundColor);
            ImGui::End();
        }
        else
        {
            camera.update();
            view = camera.getViewMatrix();
            ImGui::Begin("OpenGL Renderer");
            ImGui::Text("Mode: Render Mode.");
            ImGui::Text("FPS: %d", fpsToShow);
            ImGui::End();
        }

        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DRAW TERRAIN
        modelShader.use();;
        glm::mat4 model = glm::mat4(1.f);
        modelShader.setAllMat4(model, view, projection);

        sandTerrain.draw(modelShader);

        // DRAW WOODEN BOX
        modelShader.use();
        model = glm::mat4(1.f);

        glm::vec3 location(inputLocation[0], inputLocation[1], inputLocation[2]);
        glm::vec3 scale(inputScale[0], inputScale[1], inputScale[2]);

        model = glm::translate(model, location);
        model = glm::rotate(model, glm::radians(inputRotation[0]), glm::vec3(1.f, 0.f, 0.f));
        model = glm::rotate(model, glm::radians(inputRotation[1]), glm::vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, glm::radians(inputRotation[2]), glm::vec3(0.f, 0.f, 1.f));
        model = glm::scale(model, scale);

        modelShader.setAllMat4(model, view, projection);

        woodenBox.draw(modelShader);

        // DRAW GRIDS
        gridShader.use();
        model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);

        grid.draw(gridShader);
  
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