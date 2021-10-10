#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "FramebufferObject.h"
#include "MSFramebufferObject.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"
#include "Skybox.h"
#include "PointLight.h"

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

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Window", glfwGetPrimaryMonitor(), NULL);
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
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    Camera camera(window);
    glm::vec3 cameraPosition = camera.getCameraPosition();
    camera.setCameraSpeed(2.f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.f);
    
    MSFramebufferObject msFramebuffer(window, 8u);

    std::vector<float> quadVertices = {
        -1.f,-1.f, 0.f, 0.f, //Bottom left
         1.f,-1.f, 1.f, 0.f, //Bottom right
        -1.f, 1.f, 0.f, 1.f, //Top left

        -1.f, 1.f, 0.f, 1.f, //Top left
         1.f,-1.f, 1.f, 0.f, //Bottom right
         1.f, 1.f, 1.f, 1.f //Top right
    };
    FramebufferObject postProcessFrameBuffer(window, screenWidth,screenHeight);

    Shader frameBufferShader("shaders/fbo.vert", "shaders/fbo.frag");

    unsigned int fboVAO;
    glGenVertexArrays(1, &fboVAO);
    glBindVertexArray(fboVAO);

    unsigned int fboVBO;
    glGenBuffers(1, &fboVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fboVBO);

    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    frameBufferShader.setInt("screenTexture", 0);

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
    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Model sphere("resources/models/sphere/sphere.obj");
    Model house("resources/models/house/house.obj");
    Model woodenFloor("resources/models/woodenFloor/woodenFloor.obj");

    lightingShader.use();
    lightingShader.setFloat("material.shininess", 512.f);
    lightingShader.setBool("isBlinn", false);

    frameBufferShader.use();
    frameBufferShader.setBool("isGammaCorrectionEnabled", true);

    lightSourceShader.use();
    glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
    lightSourceShader.setVec3("light.color", lightColor);

    PointLight pLightOne(lightingShader);
    pLightOne.setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
    pLightOne.setDiffuse(glm::vec3(7.f, 7.f, 7.f));
    glm::vec3 lightPosition = glm::vec3(0.f, 25.f, 0.f);
    pLightOne.setPosition(lightPosition);

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
        cameraPosition = camera.getCameraPosition();

        lightingShader.use();
        lightingShader.setVec3("viewPosition", cameraPosition);

        if (glfwGetKey(window,GLFW_KEY_B))
        {
            lightingShader.setBool("isBlinn", true);
        }
        if (glfwGetKey(window, GLFW_KEY_P))
        {
            lightingShader.setBool("isBlinn", false);
        }

        frameBufferShader.use();
        if (glfwGetKey(window,GLFW_KEY_G))
        {
            frameBufferShader.setBool("isGamma", true);
        }
        if (glfwGetKey(window, GLFW_KEY_N))
        {
            frameBufferShader.setBool("isGamma", false);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear default buffer.

        msFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear multisampled buffer.

        lightingShader.use();
        glm::mat4 model = glm::mat4(1.f);
        lightingShader.setAllMat4(model, view, projection);
        woodenFloor.draw(lightingShader);

        lightSourceShader.use();
        model = glm::mat4(1.f);
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
        lightSourceShader.setAllMat4(model, view, projection);
        sphere.draw(lightSourceShader);

        //Be sure to draw skybox last even in all conditions.
        //skybox.draw();

        msFramebuffer.unbind();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, msFramebuffer.getFramebufferObjectId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessFrameBuffer.getFramebufferObjectId());
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Bind default framebuffer.
        
        frameBufferShader.use();
        glBindVertexArray(fboVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcessFrameBuffer.getTextureId());

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