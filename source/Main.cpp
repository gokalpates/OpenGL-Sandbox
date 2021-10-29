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
#include "DirectionalLight.h"

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
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    MSFramebufferObject msFramebuffer(window, 8u);
    
    /*
    Create shadow mapping framebuffer.
    FramebufferObject shadowMapFramebuffer(window, screenWidth, screenHeight); This is currently invalid because of no need to color attachment.
     We have to create ourselves.
    */

    int shadowResolution = 2048;

    unsigned int shadowMapFramebuffer = 0u;
    glGenFramebuffers(1, &shadowMapFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);

    unsigned int depthMap = 0u;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)(0));

    //Reference used nearest option for filters. Also, used repeat parameter instead of clamp to edge.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    
    //Specify that this framebuffer has no color attachment.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE); //Maybe we dont need this one. Try without this statement.

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Designate light position.
    glm::vec3 lightPosition = glm::vec3(10.f, 10.f, 20.f);
    glm::vec3 lightDirection = glm::normalize(glm::vec3(-lightPosition.x, -lightPosition.y, -lightPosition.z));

    //Create orthogonal perspective matrix and view matrix located at directional light's position.
    glm::mat4 shadowMapOrhthogonal = glm::ortho(-15.f, 15.f, -15.f, 15.f, 0.1f, 1000.f); //Reference used |10.f| instead of |1.f|. Why?
    glm::mat4 shadowMapView = glm::lookAt(lightPosition, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 lightSpaceMatrix = shadowMapOrhthogonal * shadowMapView;

    //Create a basic shader to generate shadow map.
    Shader shadowMapShader("shaders/shadowMap.vert", "shaders/shadowMap.frag");

    Camera camera(window);
    camera.setCameraSpeed(15.f);

    glm::vec3 cameraPosition = camera.getCameraPosition();
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

    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Model sphere("resources/models/sphere/sphere.obj");
    Model woodenBox("resources/models/Wooden Box/woodenBox.obj");
    Model woodenFloor("resources/models/Wooden Floor/woodenFloor.obj");

    //Bind temple textures manually.
    Model temple("resources/models/Temple/Temple.fbx");
    unsigned int templeDiffuse = loadTextureFromDisk("resources/models/Temple/Temple_Marble_BaseColor.png");
    unsigned int templeSpecular = loadTextureFromDisk("resources/models/Temple/Temple_Marble_Metallic.png");

    //Prepare lighting shader.
    lightingShader.use();
    lightingShader.setFloat("material.shininess", 8.f);
    //Bind shadow map at texture slot 15 to avoid overlap with other textures. Note: OpenGL guarantees 16 texture slots.
    lightingShader.setInt("shadowMap", 15);

    lightSourceShader.use();
    glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
    lightSourceShader.setVec3("light.color", lightColor);

    DirectionalLight dLightOne(lightingShader);
    dLightOne.setDirection(lightDirection);

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

        lightPosition.x = glm::cos(glfwGetTime()) * 10.f;

        shadowMapView = glm::lookAt(lightPosition, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        lightSpaceMatrix = shadowMapOrhthogonal * shadowMapView;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear default buffer.

        msFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear multisampled buffer.

        //Bind shadow mapping framebuffer and generate shadow map for each frame.
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);
        glClear(GL_DEPTH_BUFFER_BIT); //Clear depth buffer because we only have depth attachment.
        
        glViewport(0, 0, shadowResolution, shadowResolution);

        //Use shadowMap shader and draw scene.
        shadowMapShader.use();
        shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, 4.2f, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        shadowMapShader.setMat4("model", model);
        temple.draw(shadowMapShader);

        model = glm::mat4(1.f);
        model = glm::scale(model, glm::vec3(20.f, 1.f, 20.f));
        shadowMapShader.setMat4("model", model);
        woodenFloor.draw(shadowMapShader);

        //Be aware that we are not drawing light sphere! We only draw objects these litten by directional light.
        //Sphere that represents light would be in front of everything in depth buffer.

        //Return to multisampled framebuffer and draw scene.
        msFramebuffer.bind();

        glViewport(0, 0, screenWidth, screenHeight);

        //Set shader globals and shadow related stuff.
        lightingShader.use();
        lightingShader.setVec3("viewPosition", cameraPosition);
        lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        //Bind shadow map.
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        model = glm::mat4(1.f);
        model = glm::translate(model , glm::vec3(0.f, 4.2f, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        lightingShader.setAllMat4(model, view, projection);
        lightingShader.setInt("material.diffuse0", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, templeDiffuse);
        lightingShader.setInt("material.specular0", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, templeSpecular);
        temple.draw(lightingShader);

        model = glm::mat4(1.f);
        model = glm::scale(model, glm::vec3(20.f, 1.f, 20.f));
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
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        counter++;
    }
    glDeleteFramebuffers(1, &shadowMapFramebuffer);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}