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

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
int fpsToShow = 0;
size_t counter = 0;

int main()
{
    const int screenWidth = 2560, screenHeight = 1440;

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
    
    //MVP Matrices and view position vector.
    glm::mat4 model = glm::mat4(1.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::vec3 viewPosition = glm::vec3(0.f);
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)(screenWidth) / (float)(screenHeight), 0.1f, 100.f);

    //Camera.
    Camera camera(window);
    
    //Grid system.
    Grid grid;
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");

    //General States.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    //Models
    Model box("resources/objects/box/box.obj");
    Model sphere("resources/objects/sphere/sphere.obj");

    //Lighting shader.
    Shader mainShader("shaders/main.vert", "shaders/main.frag");
    mainShader.use();
    mainShader.setFloat("material.shininess", 8.f);

    //Lights
    std::vector<glm::vec3> lightPositions = {
        glm::vec3(3.f,3.f,3.f),
        glm::vec3(-3.f,3.f,3.f),
        glm::vec3(-3.f,3.f,-3.f),
        glm::vec3(3.f,3.f,-3.f),
        glm::vec3(0.f,-3.f,0.f)
    };

    std::vector<glm::vec3> lightColors = {
        glm::vec3(0.f,0.f,100.f),
        glm::vec3(0.f,100.f,0.f),
        glm::vec3(0.f,100.f,100.f),
        glm::vec3(100.f,0.f,0.f),
        glm::vec3(100.f,100.f,100.f)
    };

    for (size_t i = 0; i < lightPositions.size(); i++)
    {
        std::string uniformName = "pointLights[" + std::to_string(i) + "].position";
        char* uniformNamePtr = (char*)uniformName.c_str();
        mainShader.setVec3(uniformNamePtr, lightPositions[i]);

        uniformName = "pointLights[" + std::to_string(i) + "].color";
        uniformNamePtr = (char*)uniformName.c_str();
        mainShader.setVec3(uniformNamePtr, lightColors[i]);
    }

    //Light source.
    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");

    //Multi Sampling Anti Aliasing and HDR FBO.
    unsigned int sampleCount = 8;

    unsigned int mainFBO;
    glGenFramebuffers(1, &mainFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

    unsigned int mixedColorBuffer; //Mixed refers MSAA and HDR going to be combined.
    glGenTextures(1, &mixedColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mixedColorBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, GL_RGBA16F, screenWidth, screenHeight, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mixedColorBuffer, 0);

    unsigned int brightColorBuffer;
    glGenTextures(1, &brightColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, brightColorBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, GL_RGBA16F, screenWidth, screenHeight, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, brightColorBuffer, 0);

    unsigned int depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    {
        GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error on Framebuffer: " << std::endl;
        std::exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    unsigned int intermediateColorBuffer;
    glGenTextures(1, &intermediateColorBuffer);
    glBindTexture(GL_TEXTURE_2D, intermediateColorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateColorBuffer, 0);

    unsigned int intermediateBrightBuffer;
    glGenTextures(1, &intermediateBrightBuffer);
    glBindTexture(GL_TEXTURE_2D, intermediateBrightBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, intermediateBrightBuffer, 0);

    unsigned int intermediateDepthBuffer;
    glGenRenderbuffers(1, &intermediateDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, intermediateDepthBuffer);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, intermediateDepthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error on Framebuffer: " << std::endl;
        std::exit(1);
    }

    //Shader for quad rendering.
    Shader fboShader("shaders/fbo.vert", "shaders/fbo.frag");
    fboShader.use();
    fboShader.setInt("hdrBuffer", 0);
    fboShader.setInt("bloomBuffer", 1);
    fboShader.setFloat("exposure", 1.f);
    fboShader.setBool("bloom", false);

    unsigned int blurFBO;
    glGenFramebuffers(1, &blurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);

    unsigned int blurColorBuffer;
    glGenTextures(1, &blurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, blurColorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorBuffer, 0);

    unsigned int blurDepthBuffer;
    glGenRenderbuffers(1, &blurDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, blurDepthBuffer);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, blurDepthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error on Framebuffer: " << std::endl;
        std::exit(1);
    }

    //Quad for rendering FBOs.
    std::vector<float> quadBuffer = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    unsigned int quadVBO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(GL_ARRAY_BUFFER, quadBuffer.size() * sizeof(float), quadBuffer.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    Shader blurringShader("shaders/fbo.vert", "shaders/fboKernelBlur.frag");
    blurringShader.use();
    blurringShader.setInt("brightBuffer", 0);

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

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window, GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_E))
        {
            fboShader.use();
            fboShader.setBool("bloom", true);
        }
        if (glfwGetKey(window, GLFW_KEY_R))
        {
            fboShader.use();
            fboShader.setBool("bloom", false);
        }

        camera.update();
        view = camera.getViewMatrix();
        viewPosition = camera.getCameraPosition();

        glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Necessary lighting shader updates.
        mainShader.use();
        mainShader.setVec3("viewPosition", viewPosition);

        model = glm::mat4(1.f);
        mainShader.setAllMat4(model, view, projection);
        box.draw(mainShader);

        //Light source.
        lightSourceShader.use();
        for (size_t i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.f);
            model = glm::translate(model, lightPositions.at(i));
            model = glm::scale(model, glm::vec3(0.2f));
            lightSourceShader.setAllMat4(model, view, projection);
            lightSourceShader.setVec3("light.color", lightColors.at(i));
            box.draw(lightSourceShader);
        }

        gridShader.use();
        model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);
        grid.draw(gridShader);

        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);

        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        blurringShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, intermediateBrightBuffer);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fboShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, intermediateColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurColorBuffer);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}