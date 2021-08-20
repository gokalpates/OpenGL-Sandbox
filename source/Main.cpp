#include <iostream>
#include <string>

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

    Camera camera(window);
    camera.setCameraSpeed(5.f);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Shader modelShader("shaders/model.vert", "shaders/model.frag");

    Model sandTerrain("resources/models/Sand Terrain/sandTerrain.obj");
    Model woodenBox("resources/models/Wooden Box/woodenBox.obj");

    std::vector<float> quad = {
        -1, 1, 0, 1,
        -1,-1, 0, 0,
         1,-1, 1, 0,

        -1, 1, 0, 1,
         1,-1, 1, 0,
         1, 1, 1, 1,
    };

    // Create VAO and VBO for FBO.
    VertexArrayObject VAO;
    VAO.bind();

    VertexBufferObject VBO(quad);
    VBO.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    VBO.unbind();
    VAO.unbind();

    Shader FBOShader("shaders/fbo.vert", "shaders/fbo.frag");
    FramebufferObject FBO(window, screenWidth, screenHeight);

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

        FBO.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // DRAW TERRAIN
        modelShader.use();
        glm::mat4 model = glm::mat4(1.f);

        modelShader.setAllMat4(model, view, projection);
        sandTerrain.draw(modelShader);

        // DRAW WOODEN BOX
        modelShader.use();
        model = glm::mat4(1.f);

        model = glm::translate(model, glm::vec3(0.f, 1.11f, 0.f));

        modelShader.setAllMat4(model, view, projection);
        woodenBox.draw(modelShader);


        FBO.unbind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        FBOShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, FBO.getTextureId());
        FBOShader.setInt("screenTexture", 0);
        
        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

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