#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MSFramebufferObject.h"
#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"
#include "DirectionalLight.h"
#include "PointLight.h"

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
int fpsToShow = 0;
size_t counter = 0;

int main()
{
    int screenWidth = 2560, screenHeight = 1440;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Window", nullptr, NULL);
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

    //General States.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);

    //OGL primitives.
    Camera camera(window);
    MSFramebufferObject MSFramebuffer(window);
    Grid grid;

    //Shaders.
    Shader lightingShader("shaders/basicLight.vert", "shaders/basicLight.frag");
    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");

    //Matrices and variables.
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 model = glm::mat4(1.f);
    glm::vec3 cameraPosition = glm::vec3(0.f);

    //Light.
    glm::vec3 lightPosition = glm::vec3(3.f, 3.f, 3.f);
    glm::vec3 lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 lightDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec3 lightSpecular = glm::vec3(1.f, 1.f, 1.f);

    //Resources.
    Model sphere("resources/objects/sphere/sphere.obj");
    Model plane("resources/objects/plane/plane.obj");

    //Textures.
    stbi_set_flip_vertically_on_load(true);
    unsigned int diffuseTexture = loadTextureFromDisk("resources/textures/toy_box_diffuse.png");
    unsigned int normalTexture = loadTextureFromDisk("resources/textures/toy_box_normal.png");
    unsigned int depthTexture = loadTextureFromDisk("resources/textures/toy_box_disp.png");

    //Set light uniforms.
    lightingShader.use();
    lightingShader.setVec3("light.ambient", lightAmbient);
    lightingShader.setVec3("light.diffuse", lightDiffuse);
    lightingShader.setVec3("light.specular", lightSpecular);
    lightingShader.setVec3("light.position", lightPosition);
    lightingShader.setFloat("material.shininess", 4.f);
    lightingShader.setBool("scene.normalMapEnabled", true);
    lightingShader.setBool("scene.parallaxMapEnabled", true);

    lightingShader.setInt("material.diffuse0", 0);
    lightingShader.setInt("material.normal0", 2);
    lightingShader.setInt("material.parallax0", 3);

    lightSourceShader.use();
    lightSourceShader.setVec3("light.color", lightSpecular);

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
        cameraPosition = camera.getCameraPosition();

        lightingShader.use();
        lightingShader.setVec3("light.position", lightPosition);
        lightingShader.setVec3("scene.viewPosition", cameraPosition);

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window,GLFW_KEY_E))
            lightingShader.setBool("scene.normalMapEnabled", true);
        if (glfwGetKey(window, GLFW_KEY_R))
            lightingShader.setBool("scene.normalMapEnabled", false);
        if (glfwGetKey(window, GLFW_KEY_T))
            lightingShader.setBool("scene.parallaxMapEnabled", true);
        if (glfwGetKey(window, GLFW_KEY_Y))
            lightingShader.setBool("scene.parallaxMapEnabled", false);

        MSFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        lightingShader.use();
        model = glm::mat4(1.f);
        model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        lightingShader.setAllMat4(model, view, projection);
        plane.draw(lightingShader);

        lightSourceShader.use();
        model = glm::mat4(1.f);
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        lightSourceShader.setAllMat4(model, view, projection);
        sphere.draw(lightSourceShader);

        gridShader.use();
        model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);
        grid.draw(gridShader);

        MSFramebuffer.unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFramebuffer.getFramebufferObjectId());
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;
    }

    glDeleteTextures(1, &diffuseTexture);
    glDeleteTextures(1, &normalTexture);

    glfwDestroyWindow(window);
    glfwTerminate();
}